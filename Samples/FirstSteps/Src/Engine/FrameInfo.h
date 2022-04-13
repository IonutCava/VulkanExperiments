#pragma once

#include "Utilities/Camera.h"
#include "GameObject.h"

#include <vulkan/vulkan.h>

namespace Divide {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map& gameObjects;
    };
}; //namespace Divide