#ifndef PIPELINE_BUILDER
#define PIPELINE_BUILDER

#include "libs.h"

typedef struct _PipelineLayoutBuilder
{
    VkDevice device;
    uint32_t descSetCount;
    uint32_t _descSetCap;
    VkDescriptorSetLayout* descSetlayouts;
    uint32_t pushConstCount;
    uint32_t _pushConstCap;
    VkPushConstantRange* pushConstRanges;
} PipelineLayoutBuilder;

typedef struct _PipelineBuilder
{
    VkDevice device;
    VkPipelineShaderStageCreateInfo* shaderStages;
    uint32_t shaderCount;
    uint32_t _shaderCap;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineLayout layout;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineRenderingCreateInfo renderInfo;
    VkFormat colorFormat;
} PipelineBuilder;

PipelineLayoutBuilder* Start_PipelineLayoutBuilder(VkDevice device);

void PllBuilder_Add_DescriptorSet(PipelineLayoutBuilder* builder, VkDescriptorSetLayout layout);

void PllBuilder_Add_PushConstRange(PipelineLayoutBuilder* builder, uint32_t offset, uint32_t size, VkShaderStageFlags stage);

VkPipelineLayout Finish_PipelineLayoutBuilder(PipelineLayoutBuilder* builder);

PipelineBuilder* Start_PipelineBuilder(VkDevice device);

void PlBuilder_Set_FragmentShader(PipelineBuilder* builder, VkShaderModule shader);

void PlBuilder_Set_VertexShader(PipelineBuilder* builder, VkShaderModule shader);

void PlBuilder_Set_ColorFormat(PipelineBuilder* builder, VkFormat colorFormat);

void PlBuilder_Set_DepthFormat(PipelineBuilder* builder, VkFormat depthFormat);

void PlBuilder_Set_StencilFormat(PipelineBuilder* builder, VkFormat stencilFormat);

void PlBuilder_SetLayout(PipelineBuilder* builder, VkPipelineLayout layout);

VkPipeline Finish_PipelineBuilder(PipelineBuilder* builder);

#endif 