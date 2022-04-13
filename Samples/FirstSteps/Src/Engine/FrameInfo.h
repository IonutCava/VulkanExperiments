#pragma once

#include "Utilities/Camera.h"
#include "GameObject.h"

#include <vulkan/vulkan.h>

namespace Divide {
    constexpr uint32_t MAX_LIGHTS = 10u;

    struct PointLight {
        glm::vec4 position{};  //w is unused;
        glm::vec4 colour{}; //w is intensity;
    };

    struct GlobalUbo {
        glm::mat4 projectionMatrix{ 1.f };
        glm::mat4 viewMatrix{ 1.f };
        glm::vec4 ambientLightColour{ 1.f, 1.f, 1.f, .02f };
        PointLight pointLights[MAX_LIGHTS];
        int numLights = 0;
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map& gameObjects;
    };
}; //namespace Divide