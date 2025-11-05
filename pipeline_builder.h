#ifndef PIPELINE_BUILDER
#define PIPELINE_BUILDER

#include "libs.h"

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
    VkPipelineLayout pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineRenderingCreateInfo renderInfo;
} PipelineBuilder;

#endif 