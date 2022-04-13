#pragma once

#include "../Utilities/Camera.h"

#include <vulkan/vulkan.h>

namespace Divide {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
    };
}; //namespace Divide