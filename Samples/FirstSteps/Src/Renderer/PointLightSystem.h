#pragma once

#include "Utilities/Pipeline.h"
#include "Utilities/Device.h"
#include "Utilities/Model.h"
#include "Utilities/Camera.h"

#include "Engine/FrameInfo.h"
#include "Engine/GameObject.h"

#include <memory>

namespace Divide {
    class PointLightSystem {
    public:
        PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;
        PointLightSystem(PointLightSystem&&) = delete;
        PointLightSystem& operator=(PointLightSystem&&) = delete;

        void render(FrameInfo& frameInf);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device& _device;

        std::unique_ptr<Pipeline> _pipelinePtr;
        VkPipelineLayout _pipelineLayout;
    };
}; //namespace Divide