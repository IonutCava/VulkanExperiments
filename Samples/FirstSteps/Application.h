#pragma once

#include "Utilities/Window.h"
#include "Utilities/Pipeline.h"
#include "Utilities/Device.h"
#include "Utilities/SwapChain.h"
#include "Utilities/Model.h"

#include <memory>

namespace Divide {
    class Application {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application&) = delete;
        void operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void run();
    private:
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        Window _window{WIDTH, HEIGHT, "Hiya Vulkan"};
        Device _device{_window};
        SwapChain _swapChain{ _device, _window.getExtent() };
        std::unique_ptr<Pipeline> _pipelinePtr;
        VkPipelineLayout _pipelineLayout;
        std::vector<VkCommandBuffer> _commandBuffers;
        std::unique_ptr<Model> _modelPtr;
    };
}; //namespace Divide