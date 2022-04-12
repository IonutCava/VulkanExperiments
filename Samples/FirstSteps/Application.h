#pragma once

#include "Utilities/Window.h"
#include "Utilities/Pipeline.h"
#include "Utilities/Device.h"
#include "Utilities/SwapChain.h"
#include "Utilities/Model.h"
#include "Engine/GameObject.h"

#include <memory>

namespace Divide {
    class Application {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void run();

    private:
        void loadGameObjects();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderGameObjects(VkCommandBuffer commandBuffer);

        Window _window{WIDTH, HEIGHT, "Hiya Vulkan"};
        Device _device{_window};
        std::unique_ptr<SwapChain> _swapChainPtr;
        std::unique_ptr<Pipeline> _pipelinePtr;
        VkPipelineLayout _pipelineLayout;
        std::vector<VkCommandBuffer> _commandBuffers;
        std::vector<GameObject> _gameObjects;
    };
}; //namespace Divide