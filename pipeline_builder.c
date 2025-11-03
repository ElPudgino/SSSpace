#include "engine_init.h"

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

PipelineBuilder* PipelineBuilder_Start(EngineState* engineState)
{
    PipelineBuilder* res = (PipelineBuilder*)calloc(1, sizeof(PipelineBuilder));
    res->_shaderCap = 2;
    res->device = engineState->device;
    res->shaderStages = (VkPipelineShaderStageCreateInfo*)calloc(2, sizeof(VkPipelineShaderStageCreateInfo));
    res->inputAssembly = (VkPipelineInputAssemblyStateCreateInfo){.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,.sType= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    res->rasterizer = (VkPipelineRasterizationStateCreateInfo){.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    res->colorBlendAttachment = (VkPipelineColorBlendAttachmentState){};
    res->multisampling = (VkPipelineMultisampleStateCreateInfo){.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    res->depthStencil = (VkPipelineDepthStencilStateCreateInfo){
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthWriteEnable = VK_TRUE,
        .depthTestEnable = VK_TRUE,
        .depthBoundsTestEnable = VK_FALSE,.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    res->renderInfo.colorAttachmentCount = 1;
    return res;
}

int PlBuilder_Set_ColorFormat(PipelineBuilder* builder, VkFormat colorFormat)
{
    builder->renderInfo.pColorAttachmentFormats = &colorFormat;
    return 1;
}

int PlBuilder_Set_DepthFormat(PipelineBuilder* builder, VkFormat depthFormat)
{
    builder->renderInfo.depthAttachmentFormat = depthFormat;
    return 1;
}

int PlBuilder_Set_StencilFormat(PipelineBuilder* builder, VkFormat stencilFormat)
{
    builder->renderInfo.stencilAttachmentFormat = stencilFormat;
    return 1;
}

int PlBuilder_Set_PipelineLayout(PipelineBuilder* builder, VkPipelineLayout layout)
{
    builder->pipelineLayout = layout;
    return 1;
}

VkPipeline PipelineBuilder_Finish(PipelineBuilder* builder)
{
    VkGraphicsPipelineCreateInfo cInfo = {};
    
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = NULL;

    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = NULL;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &builder->colorBlendAttachment;

    VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicInfo.pDynamicStates = &state[0];
    dynamicInfo.dynamicStateCount = 2;

    VkPipelineVertexInputStateCreateInfo _vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

    cInfo.pNext = &builder->renderInfo;

    cInfo.stageCount = builder->shaderCount;
    cInfo.pStages = builder->shaderStages;
    cInfo.pVertexInputState = &_vertexInputInfo;
    cInfo.pInputAssemblyState = &builder->inputAssembly;
    cInfo.pViewportState = &viewportState;
    cInfo.pRasterizationState = &builder->rasterizer;
    cInfo.pMultisampleState = &builder->multisampling;
    cInfo.pColorBlendState = &colorBlending;
    cInfo.pDepthStencilState = &builder->depthStencil;
    cInfo.layout = builder->pipelineLayout;
    cInfo.pDynamicState = &dynamicInfo;

    VkPipeline res;
    if (vkCreateGraphicsPipelines(builder->device, NULL, 1, &cInfo, NULL, &res) != VK_SUCCESS)
    {
        printf("Failed to create graphics pipeline\n");
        return NULL;
    }
    return res;
}