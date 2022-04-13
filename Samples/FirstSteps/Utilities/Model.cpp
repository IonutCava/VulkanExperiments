#include "Model.h"

#include "Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <unordered_map>

namespace std {
    template<>
    struct hash<Divide::Model::Vertex> {
        size_t operator()(Divide::Model::Vertex const& vertex) const {
            size_t seed = 0;
            Divide::hashCombine(seed, vertex.position, vertex.colour, vertex.normal, vertex.uv);
            return seed;
        }
    };
};

namespace Divide {

    Model::Model(Device& device, const Builder& builder)
        : _device(device)
    {
        createVertexBuffers(builder._vertices);
        createIndexBuffers(builder._indices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(_device.device(), _vertexBuffer, nullptr);
        vkFreeMemory(_device.device(), _vertexBufferMemory, nullptr);

        if (_hasIndexBuffer) {
            vkDestroyBuffer(_device.device(), _indexBuffer, nullptr);
            vkFreeMemory(_device.device(), _indexBufferMemory, nullptr);
        }
    }

    std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string& filePath) {
        Builder builder{};
        builder.loadModel(filePath);
        return std::make_unique<Model>(device, builder);
    }

    void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
        _vertexCount = static_cast<uint32_t>(vertices.size());
        assert(_vertexCount >= 3 && "Vertex count must be at least 3");

        const VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        
        _device.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer,
                             stagingBufferMemory); 
        void* data;
        vkMapMemory(_device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device.device(), stagingBufferMemory);

        _device.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             _vertexBuffer,
                             _vertexBufferMemory);

        _device.copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(_device.device(), stagingBuffer, nullptr);
        vkFreeMemory(_device.device(), stagingBufferMemory, nullptr);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
        _indexCount = static_cast<uint32_t>(indices.size());
        _hasIndexBuffer = _indexCount > 0u;

        if (!_hasIndexBuffer) {
            return;
        }

        const VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        
        _device.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer,
                             stagingBufferMemory); 
        void* data;
        vkMapMemory(_device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device.device(), stagingBufferMemory);

        _device.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             _indexBuffer,
                             _indexBufferMemory);

        _device.copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

        vkDestroyBuffer(_device.device(), stagingBuffer, nullptr);
        vkFreeMemory(_device.device(), stagingBufferMemory, nullptr);
    }

    void Model::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { _vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (_hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::draw(VkCommandBuffer commandBuffer) {
        if (_hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, colour);
        return attributeDescriptions;
    }

    void Model::Builder::loadModel(const std::string& filePath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        _vertices.clear();
        _indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    auto colorIndex = 3 * index.vertex_index + 2;
                    if (colorIndex < attrib.colors.size()) {
                        vertex.colour = {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex - 0]
                        };
                    } else {
                        vertex.colour = { 1.f, 1.f, 1.f };
                    }
                }
                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                    _vertices.push_back(vertex);
                }

                _indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}; //namespace Divide
