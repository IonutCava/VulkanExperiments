#include "Application.h"
#include "Engine/SimpleRenderSystem.h"
#include "Utilities/Camera.h"
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
        loadGameObjects();
    }

    Application::~Application()
    {
    }

    void Application::run() {
        SimpleRenderSystem simpleRenderSystem{ _device, _renderer.getSwapChainRenderPass() };
        Camera camera{};

        auto viewerObject = GameObject::CreateGameObject();
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
                _renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, _gameObjects, camera);
                _renderer.endSwapChainRenderPass(commandBuffer);
                _renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(_device.device());
    }

    void Application::loadGameObjects() {
        std::shared_ptr<Model> model = Model::createModelFromFile(_device, "Assets/Models/smooth_vase.obj");
        auto gameObject = GameObject::CreateGameObject();
        gameObject._model = model;
        gameObject._transform.translation = { .0f, .0f, 2.5f };
        gameObject._transform.scale = glm::vec3(3.f);
        _gameObjects.push_back(std::move(gameObject));
    }
}; //namespace Divide