#include "Application.h"
#include "Engine/SimpleRenderSystem.h"
#include "Utilities/Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

constexpr bool USE_ORTHO = false;

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
        //camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ .5f, 0.f, 1.f });
        camera.setViewTarget(glm::vec3(-1.f, -2.f, -20.f), glm::vec3(0.f, 0.f, 2.5f));

        while (!_window.shouldClose()) {
            glfwPollEvents();

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
            std::vector<Model::Vertex> vertices{

                // left face (white)
                {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
                {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
                {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
                {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

                // right face (yellow)
                {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
                {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
                {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
                {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

                // top face (orange, remember y axis points down)
                {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
                {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
                {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

                // bottom face (red)
                {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
                {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
                {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

                // nose face (blue)
                {{-.5f, -.5f, 0.5f}, {.1f, .3f, .8f}},
                {{.5f, .5f, 0.5f}, {.1f, .3f, .8f}},
                {{-.5f, .5f, 0.5f}, {.1f, .3f, .8f}},
                {{-.5f, -.5f, 0.5f}, {.1f, .3f, .8f}},
                {{.5f, -.5f, 0.5f}, {.1f, .3f, .8f}},
                {{.5f, .5f, 0.5f}, {.1f, .3f, .8f}},

                // tail face (green)
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

            };
            for (auto& v : vertices) {
                v.position += offset;
            }
            return std::make_unique<Model>(device, vertices);
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