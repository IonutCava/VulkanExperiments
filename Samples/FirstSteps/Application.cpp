#include "Application.h"
#include "Engine/SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace Divide {

    Application::Application()
    {
        loadGameObjects();
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        SimpleRenderSystem simpleRenderSystem{ _device, _renderer.getSwapChainRenderPass() };
        while (!_window.shouldClose()) {
            glfwPollEvents();
            if (auto commandBuffer = _renderer.beginFrame()) {
                _renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, _gameObjects);
                _renderer.endSwapChainRenderPass(commandBuffer);
                _renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(_device.device());
    }

    namespace {
        constexpr glm::vec3 red {1.f, 0.f, 0.f};
        constexpr glm::vec3 green {0.f, 1.f, 0.f};
        constexpr glm::vec3 yellow {1.f, 1.f, 0.f};

        void sierpinski(std::vector<Model::Vertex>& vertices,
                        int depth,
                        glm::vec2 left,
                        glm::vec2 right,
                        glm::vec2 top)
        {
            if (depth <= 0) {
                vertices.push_back({ top, red });
                vertices.push_back({ right, green });
                vertices.push_back({ left, yellow });
            } else {
                auto leftTop = 0.5f * (left + top);
                auto rightTop = 0.5f * (right + top);
                auto leftRight = 0.5f * (left + right);
                sierpinski(vertices, depth - 1, left, leftRight, leftTop);
                sierpinski(vertices, depth - 1, leftRight, right, rightTop);
                sierpinski(vertices, depth - 1, leftTop, rightTop, top);
            }
        };
    };

    void Application::loadGameObjects() {
        std::vector<Model::Vertex> vertices{};
        sierpinski(vertices, 2, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
        auto modelPtr = std::make_shared<Model>(_device, vertices);

        auto triangle = GameObject::CreateGameObject();
        triangle._model = modelPtr;
        triangle._colour = { .1f, .8f, .1f };
        triangle._transform2D.translation.x = .2f;
        triangle._transform2D.scale = { 2.f, 3.f };
        triangle._transform2D.rotation = .25f * glm::two_pi<float>();
        _gameObjects.push_back(std::move(triangle));
    }
}; //namespace Divide