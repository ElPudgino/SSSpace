#include "engine_init.h"
#include "pipeline_builder.h"
#include <stdio.h>

PipelineBuilder* PipelineBuilder_Start(EngineState* engineState)
{
    PipelineBuilder* res = (PipelineBuilder*)calloc(1, sizeof(PipelineBuilder));
    res->_shaderCap = 2;
    res->device = engineState->device;
    res->shaderStages = (VkPipelineShaderStageCreateInfo*)calloc(2, sizeof(VkPipelineShaderStageCreateInfo));
    res->inputAssembly = (VkPipelineInputAssemblyStateCreateInfo){.sType= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    res->rasterizer = (VkPipelineRasterizationStateCreateInfo){.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    res->colorBlendAttachment = (VkPipelineColorBlendAttachmentState){};
    res->multisampling = (VkPipelineMultisampleStateCreateInfo){.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT};
    res->depthStencil = (VkPipelineDepthStencilStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE};
    res->renderInfo.colorAttachmentCount = 1;
    return res;
}

void PlBuilder_Set_FragmentShader(PipelineBuilder* builder, VkShaderModule shader)
{
    if (builder->shaderCount == builder->_shaderCap) builder->shaderStages = (VkPipelineShaderStageCreateInfo*)realloc(builder->shaderStages, builder->_shaderCap * 2 * sizeof(VkPipelineShaderStageCreateInfo)),builder->_shaderCap *= 2;
    VkPipelineShaderStageCreateInfo cInfo = {};
    cInfo.module = shader;
    cInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    cInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    cInfo.pName = "main";
    builder->shaderStages[builder->shaderCount] = cInfo;
}

void PlBuilder_Set_VertexShader(PipelineBuilder* builder, VkShaderModule shader)
{
    if (builder->shaderCount == builder->_shaderCap) builder->shaderStages = (VkPipelineShaderStageCreateInfo*)realloc(builder->shaderStages, builder->_shaderCap * 2 * sizeof(VkPipelineShaderStageCreateInfo)),builder->_shaderCap *= 2;
    VkPipelineShaderStageCreateInfo cInfo = {};
    cInfo.module = shader;
    cInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    cInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    cInfo.pName = "main";
    builder->shaderStages[builder->shaderCount] = cInfo;
}

void PlBuilder_Set_ColorFormat(PipelineBuilder* builder, VkFormat colorFormat)
{
    builder->renderInfo.pColorAttachmentFormats = &colorFormat;
}

void PlBuilder_Set_DepthFormat(PipelineBuilder* builder, VkFormat depthFormat)
{
    builder->renderInfo.depthAttachmentFormat = depthFormat;
}

void PlBuilder_Set_StencilFormat(PipelineBuilder* builder, VkFormat stencilFormat)
{
    builder->renderInfo.stencilAttachmentFormat = stencilFormat;
}

void PlBuilder_Set_PipelineLayout(PipelineBuilder* builder, VkPipelineLayout layout)
{
    builder->pipelineLayout = layout;
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
        free(builder->shaderStages);
        free(builder);
        return NULL;
    }
    free(builder->shaderStages);
    free(builder);
    return res;
}