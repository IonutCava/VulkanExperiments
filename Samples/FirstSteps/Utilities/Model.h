#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace Divide {
    class Model {
    public:
        struct Vertex {
            glm::vec2 position;
            glm::vec3 colour;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        Model() = default;
        Model(Device& device, const std::vector<Vertex>& vertices);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) = delete;
        Model& operator=(Model&&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);

    private:
        Device& _device;
        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        uint32_t _vertexCount = 0u;
    };
}; //namespace Divide
