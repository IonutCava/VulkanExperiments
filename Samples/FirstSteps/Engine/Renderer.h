#pragma once

#include "../Utilities/Window.h"
#include "../Utilities/Device.h"
#include "../Utilities/SwapChain.h"
#include "../Utilities/Model.h"

#include <memory>
#include <cassert>

namespace Divide {
    class Renderer {
    public:
        Renderer() = default;
        Renderer(Window& window, Device& device);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        [[nodiscard]] inline VkRenderPass getSwapChainRenderPass() const { return _swapChainPtr->getRenderPass(); }
        [[nodiscard]] bool isFrameInProgress() const { return _isFrameStarted; }

        [[nodiscard]] inline VkCommandBuffer getCurrentCommandBuffer() const {
            assert(_isFrameStarted && "Cannot get command buffer when frame not in progress!");
            return _commandBuffers[_currentFrameIndex];
        }

        [[nodiscard]] inline int getFrameIndex() const {
            assert(_isFrameStarted && "Cannot get frame index when frame is not in progress!");
            return _currentFrameIndex;
        }

        [[nodiscard]] VkCommandBuffer beginFrame();

        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window& _window;
        Device& _device;
        std::unique_ptr<SwapChain> _swapChainPtr;
        std::vector<VkCommandBuffer> _commandBuffers;
        uint32_t _currentImageIndex{ 0u };
        int _currentFrameIndex{ 0 };
        bool _isFrameStarted{ false };
    };
}; //namespace Divide;
