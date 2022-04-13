#include "Application.h"
#include "Renderer/SimpleRenderSystem.h"
#include "Renderer/PointLightSystem.h"
#include "Utilities/Camera.h"
#include "Utilities/Buffer.h"
#include "Engine/KeyboardInputController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

constexpr bool USE_ORTHO = false;
constexpr float MAX_FRAME_TIME = 0.33f;

namespace Divide {

    Application::Application()
    {
        _globalPoolPtr = DescriptorPool::Builder(_device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        loadGameObjects();
    }

    Application::~Application()
    {
    }

    void Application::run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); ++i) {
            uboBuffers[i] = std::make_unique<Buffer>(
                _device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(_device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); ++i) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *_globalPoolPtr)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{ _device, _renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        PointLightSystem pointLightSystem{ _device, _renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        Camera camera{};

        auto viewerObject = GameObject::CreateGameObject();
        viewerObject._transform.translation.z = -2.5f;
        KeyboardInputController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!_window.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(_window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject._transform.translation, viewerObject._transform.rotation);

            const float aspect = _renderer.getAspectRatio();
            if constexpr (USE_ORTHO) {
                camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            } else {
                camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.01f, 100.f);
            }

            if (auto commandBuffer = _renderer.beginFrame()) {
                const int frameIndex = _renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    _gameObjects
                };
                
                // update
                GlobalUbo ubo{};
                ubo.projectionMatrix = camera.getProjection();
                ubo.viewMatrix = camera.getView();
                ubo.inverseViewMatrix = camera.getInverseView();

                pointLightSystem.update(frameInfo, ubo);

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                
                // render
                _renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
                _renderer.endSwapChainRenderPass(commandBuffer);
                _renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(_device.device());
    }

    void Application::loadGameObjects() {
        {
            std::shared_ptr<Model> model = Model::createModelFromFile(_device, "Assets/Models/smooth_vase.obj");
            auto gameObject = GameObject::CreateGameObject();
            gameObject._model = model;
            gameObject._transform.translation = { .5f, .5f, 0.f };
            gameObject._transform.scale = glm::vec3(3.f, 1.5f, 2.5f);
            _gameObjects.emplace(gameObject.getId(), std::move(gameObject));
        }
        {
            std::shared_ptr<Model> model = Model::createModelFromFile(_device, "Assets/Models/flat_vase.obj");
            auto gameObject = GameObject::CreateGameObject();
            gameObject._model = model;
            gameObject._transform.translation = { -.5f, .5f, 0.f };
            gameObject._transform.scale = glm::vec3(3.f);
            _gameObjects.emplace(gameObject.getId(), std::move(gameObject));
        }
        {
            std::shared_ptr<Model> model = Model::createModelFromFile(_device, "Assets/Models/quad.obj");
            auto gameObject = GameObject::CreateGameObject();
            gameObject._model = model;
            gameObject._transform.translation = { 0.f, .5f, 0.f };
            gameObject._transform.scale = glm::vec3(3.f);
            _gameObjects.emplace(gameObject.getId(), std::move(gameObject));
        }
        
         const std::vector<glm::vec3> lightColours{
              {1.f, .1f, .1f},
              {.1f, .1f, 1.f},
              {.1f, 1.f, .1f},
              {1.f, 1.f, .1f},
              {.1f, 1.f, 1.f},
              {1.f, 1.f, 1.f}  //
         };

         for (size_t i = 0; i < lightColours.size(); ++i) {
             auto pointLight = GameObject::MakePointLight(0.2f);
             pointLight._colour = lightColours[i];
             auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColours.size(), {0.f, -1.f, 0.f});
             pointLight._transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
             _gameObjects.emplace(pointLight.getId(), std::move(pointLight));
         }
    }
}; //namespace Divide