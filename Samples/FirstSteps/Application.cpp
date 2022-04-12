#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>
#include <array>

namespace Divide {

    struct SimplePushConstantData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    Application::Application()
    {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    Application::~Application()
    {
        vkDestroyPipelineLayout(_device.device(), _pipelineLayout, nullptr);
    }

    void Application::run()
    {
        while (!_window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(_device.device());
    }

    void Application::createPipelineLayout() {

        VkPushConstantRange pushContantRange{};
        pushContantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushContantRange.offset = 0;
        pushContantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushContantRange;
        if (vkCreatePipelineLayout(_device.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
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

    void Application::loadModels() {
        std::vector<Model::Vertex> vertices{};
        sierpinski(vertices, 2, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
        _modelPtr = std::make_unique<Model>(_device, vertices);
    }

    void Application::createPipeline() {
        assert(_swapChainPtr != nullptr && "Cannot create pipeline before swap chain!");
        assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};

        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = _swapChainPtr->getRenderPass();
        pipelineConfig.pipelineLayout = _pipelineLayout;
        _pipelinePtr = std::make_unique<Pipeline>(_device, "Shaders/simple.vert.spv", "Shaders/simple.frag.spv", pipelineConfig);
    }

    void Application::createCommandBuffers() {
        _commandBuffers.resize(_swapChainPtr->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if (vkAllocateCommandBuffers(_device.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Faile to allocate command buffers");
        }
    }

    void Application::freeCommandBuffers() {
        vkFreeCommandBuffers(_device.device(), _device.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

    void Application::drawFrame() {
        uint32_t imageIndex = 0u;
        auto result = _swapChainPtr->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);
        result = _swapChainPtr->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            _window.wasWindowResized())
        {
            _window.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }
    }

    void Application::recreateSwapChain() {
        auto extent = _window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = _window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device.device());
        if (_swapChainPtr == nullptr) {
            _swapChainPtr = std::make_unique<SwapChain>(_device, extent);
        } else {
            _swapChainPtr = std::make_unique<SwapChain>(_device, extent, std::move(_swapChainPtr));
            if (_swapChainPtr->imageCount() != _commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // if render pass compatible, skip this
        createPipeline();
    }

    void Application::recordCommandBuffer(const int imageIndex) {
        static int frame = 0;
        frame = ++frame % 1000;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _swapChainPtr->getRenderPass();
        renderPassInfo.framebuffer = _swapChainPtr->getFrameBuffer(static_cast<int>(imageIndex));
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _swapChainPtr->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.8f, 1.0f };
        clearValues[1].depthStencil = { 1.f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = static_cast<float>(_swapChainPtr->getSwapChainExtent().width);
        viewport.height = static_cast<float>(_swapChainPtr->getSwapChainExtent().height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        VkRect2D scissor{ {0, 0}, _swapChainPtr->getSwapChainExtent() };

        vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);

        vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

        _pipelinePtr->bind(_commandBuffers[imageIndex]);
        _modelPtr->bind(_commandBuffers[imageIndex]);

        for (int j = 0; j < 4; j++) {
            SimplePushConstantData push{};
            push.offset = { -0.5f + frame * 0.002f, -0.4f + j * 0.25f };
            push.color = { 0.f, 0.f, 0.2f + 0.2f * j };
            vkCmdPushConstants(_commandBuffers[imageIndex], _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            _modelPtr->draw(_commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(_commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

}; //namespace Divide