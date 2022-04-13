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

    namespace {
        // temporary helper function, creates a 1x1x1 cube centered at offset
        std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
            Model::Builder modelBuilder{};

            modelBuilder._vertices = {
                // left face (white)
                {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                {{-.5f,  .5f,  .5f}, {.9f, .9f, .9f}},
                {{-.5f, -.5f,  .5f}, {.9f, .9f, .9f}},
                {{-.5f,  .5f, -.5f}, {.9f, .9f, .9f}},

                // right face (yellow)
                {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                {{.5f,  .5f,  .5f}, {.8f, .8f, .1f}},
                {{.5f, -.5f,  .5f}, {.8f, .8f, .1f}},
                {{.5f,  .5f, -.5f}, {.8f, .8f, .1f}},

                // top face (orange, remember y axis points down)
                {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                {{ .5f, -.5f,  .5f}, {.9f, .6f, .1f}},
                {{-.5f, -.5f,  .5f}, {.9f, .6f, .1f}},
                {{ .5f, -.5f, -.5f}, {.9f, .6f, .1f}},

                // bottom face (red)
                {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                {{ .5f, .5f,  .5f}, {.8f, .1f, .1f}},
                {{-.5f, .5f,  .5f}, {.8f, .1f, .1f}},
                {{ .5f, .5f, -.5f}, {.8f, .1f, .1f}},

                // nose face (blue)
                {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
                {{ .5f,  .5f, 0.5f}, {.1f, .1f, .8f}},
                {{-.5f,  .5f, 0.5f}, {.1f, .1f, .8f}},
                {{ .5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

                // tail face (green)
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{ .5f,  .5f, -0.5f}, {.1f, .8f, .1f}},
                {{-.5f,  .5f, -0.5f}, {.1f, .8f, .1f}},
                {{ .5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            };
            for (auto& v : modelBuilder._vertices) {
                v.position += offset;
            }

            modelBuilder._indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                      12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

            return std::make_unique<Model>(device, modelBuilder);
        }
    };

    void Application::loadGameObjects() {
        std::shared_ptr<Model> model = createCubeModel(_device, { .0f, .0f, .0f });
        auto cube = GameObject::CreateGameObject();
        cube._model = model;
        cube._transform.translation = { .0f, .0f, 2.5f };
        cube._transform.scale = { .5f, .5f, .5f };
        _gameObjects.push_back(std::move(cube));
    }
}; //namespace Divide