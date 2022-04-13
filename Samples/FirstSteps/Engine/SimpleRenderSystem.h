#pragma once

#include "../Utilities/Pipeline.h"
#include "../Utilities/Device.h"
#include "../Utilities/Model.h"
#include "../Utilities/Camera.h"

#include "FrameInfo.h"
#include "GameObject.h"

#include <memory>

namespace Divide {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem(SimpleRenderSystem&&) = delete;
        SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

        void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device& _device;

        std::unique_ptr<Pipeline> _pipelinePtr;
        VkPipelineLayout _pipelineLayout;
    };
}; //namespace Divide