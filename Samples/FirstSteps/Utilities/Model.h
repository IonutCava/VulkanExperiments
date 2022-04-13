#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace Divide {
    class Model {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 colour{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return position == other.position &&
                       colour == other.colour &&
                       normal == other.normal &&
                       uv == other.uv;
            }
        };

        struct Builder {
            std::vector<Vertex> _vertices{};
            std::vector<uint32_t> _indices{};

            void loadModel(const std::string& filePath);
        };

        Model() = default;
        Model(Device& device, const Builder& builder);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&&) = delete;
        Model& operator=(Model&&) = delete;

        static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filePath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

    private:
        Device& _device;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        uint32_t _vertexCount = 0u; 

        bool _hasIndexBuffer = false;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;
        uint32_t _indexCount = 0u;
    };
}; //namespace Divide
