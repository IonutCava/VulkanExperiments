#pragma once

#include "Device.h"
#include <string>
#include <vector>

namespace Divide {

    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        VkPipelineViewportStateCreateInfo viewportInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};

        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};

        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0u;
    };

    class Pipeline {
    public:
        Pipeline() = default;
        Pipeline(Device& device, const std::string& vertFile, const std::string& fragFile, const PipelineConfigInfo& configInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        Pipeline& operator=(Pipeline&&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        static std::vector<char> readFile(const std::string& filePath);

        void createGraphicsPipeline(const std::string& vertFile, const std::string& fragFile, const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        Device& _device;
        VkPipeline _graphicsPipeline;
        VkShaderModule _vertShaderModule;
        VkShaderModule _fragShaderModule;
    };
}; //namespace Divide