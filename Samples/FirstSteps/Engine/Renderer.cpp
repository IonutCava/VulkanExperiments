#include "Renderer.h"

#include <stdexcept>
#include <array>

namespace Divide {

    Renderer::Renderer(Window& window, Device& device)
        : _window{ window }, _device{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer()
    {
        freeCommandBuffers();
    }

    void Renderer::createCommandBuffers() {
        _commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if (vkAllocateCommandBuffers(_device.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Faile to allocate command buffers");
        }
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(_device.device(), _device.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

    void Renderer::recreateSwapChain() {
        auto extent = _window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = _window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device.device());
        if (_swapChainPtr == nullptr) {
            _swapChainPtr = std::make_unique<SwapChain>(_device, extent);
        } else {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapChainPtr);
            _swapChainPtr = std::make_unique<SwapChain>(_device, extent, oldSwapChain);
            if (!oldSwapChain->compareSwapFormats(*_swapChainPtr.get())) {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    [[nodiscard]] VkCommandBuffer Renderer::beginFrame() {
        assert(!_isFrameStarted && "Can't call beginFrame while already in progress!");
        auto result = _swapChainPtr->acquireNextImage(&_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image!");
        }

        _isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        return commandBuffer;
    }

    void Renderer::endFrame() {
        assert(_isFrameStarted && "Can't call endFrame while frame is not in progress!");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }

        auto result = _swapChainPtr->submitCommandBuffers(&commandBuffer, &_currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            _window.wasWindowResized())
        {
            _window.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        _isFrameStarted = false;
        _currentFrameIndex = ++_currentFrameIndex % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(_isFrameStarted && "Can't call beginSwapChainRenderPass while frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _swapChainPtr->getRenderPass();
        renderPassInfo.framebuffer = _swapChainPtr->getFrameBuffer(static_cast<int>(_currentImageIndex));
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _swapChainPtr->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.8f, 1.0f };
        clearValues[1].depthStencil = { 1.f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = static_cast<float>(_swapChainPtr->getSwapChainExtent().width);
        viewport.height = static_cast<float>(_swapChainPtr->getSwapChainExtent().height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        VkRect2D scissor{ {0, 0}, _swapChainPtr->getSwapChainExtent() };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(_isFrameStarted && "Can't call endSwapChainRenderPass while frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame!");


        vkCmdEndRenderPass(commandBuffer);
    }
}; //namespace Divide
