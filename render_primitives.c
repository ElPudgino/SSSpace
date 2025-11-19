#include "render_primitives.h"

#define PUSH_CONSTANTS_MAX_SIZE 128

void _MatBuilder_Increase_PoolSize(MaterialBuilder* builder, VkDescriptorType descType)
{
    assert(builder);
    VkDescriptorPoolSize size = {};
    switch (descType)
    {
    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        builder->poolSizes.sizesPerSet[0].descriptorCount++;
        break;
    case VK_DESCRIPTOR_TYPE_SAMPLER:
        builder->poolSizes.sizesPerSet[1].descriptorCount++;
        break;
    
    default:
        printf("!Used not implemented descriptor type in material construction\n");
    }
}

MaterialBuilder* Start_MaterialBuilder(VkDevice device)
{
    MaterialBuilder* builder = (MaterialBuilder*)calloc(1, sizeof(MaterialBuilder));
    builder->poolSizes.sizesPerSet = (VkDescriptorPoolSize*)calloc(6, sizeof(VkDescriptorPoolSize));
    builder->poolSizes.sizesCount = 6;
    builder->poolSizes.sizesPerSet[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    builder->poolSizes.sizesPerSet[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;

    builder->matParams = (MaterialParameter*)calloc(1, sizeof(MaterialParameter));

    builder->descLayoutBuilder = Start_DescriptorLayoutBuilder(device);
    builder->pipelineLayoutBuilder = Start_PipelineLayoutBuilder(device);
    builder->pipelineBuilder = Start_PipelineBuilder(device);

    PlBuilder_Set_ColorFormat(builder->pipelineBuilder, VK_FORMAT_R16G16B16A16_SFLOAT);
    PlBuilder_Set_DepthFormat(builder->pipelineBuilder, VK_FORMAT_R16_USCALED);
    PlBuilder_Set_StencilFormat(builder->pipelineBuilder, VK_FORMAT_R8_UINT);

    builder->device = device;
    return builder;
}

void MatBuilder_SetFragmentShader(MaterialBuilder* builder, VkShaderModule shader)
{  
    assert(builder);
    PlBuilder_Set_FragmentShader(builder->pipelineBuilder, shader);
}

void MatBuilder_SetVertexShader(MaterialBuilder* builder, VkShaderModule shader)
{  
    assert(builder);
    PlBuilder_Set_VertexShader(builder->pipelineBuilder, shader);
}

void MatBuilder_AddImageSlot(MaterialBuilder* builder, uint32_t bind, VkShaderStageFlags stage)
{
    assert(builder);
    DlBuilder_Add_DescriptorLayoutBinding(builder->descLayoutBuilder, bind, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, stage);
    _MatBuilder_Increase_PoolSize(builder, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
}

void MatBuilder_AddParameter(MaterialBuilder* builder, uint32_t paramSize, VkShaderStageFlags stage)
{
    assert(builder);
    PllBuilder_Add_PushConstRange(builder->pipelineLayoutBuilder, builder->curParamOffset, paramSize, stage);
    builder->matParams = (MaterialParameter*)realloc(builder->matParams, (builder->matParamsCount + 1) * sizeof(MaterialParameter));
    builder->matParams[builder->matParamsCount] = (MaterialParameter){
        .size = paramSize,
        .offset = builder->curParamOffset,
        .stage = stage,
        .value = calloc(paramSize, 1)};
    builder->matParamsCount++;
    // Paremeters are added next to each other
    builder->curParamOffset += paramSize;
    if (builder->curParamOffset > PUSH_CONSTANTS_MAX_SIZE) printf("!Total size of parameters in a material exceeds %d byte\n", PUSH_CONSTANTS_MAX_SIZE);
}

Material Finish_MaterialBuilder(MaterialBuilder* builder)
{
    assert(builder);
    Material mat = {};
    mat.parameterCount = builder->matParamsCount;
    mat.parameters = builder->matParams;
    mat.device = builder->device;
    // Order of these commands is important
    mat.ownLayout = Finish_DescriptorLayoutBuilder(builder->descLayoutBuilder, 0);
    mat.ownPool = Create_DescriptorPool(builder->device, 1, builder->poolSizes);

    PllBuilder_Add_DescriptorSet(builder->pipelineLayoutBuilder, mat.ownLayout);

    mat.descSets[3] = Allocate_DescriptorSet(builder->device, mat.ownPool, mat.ownLayout);
    mat.pLayout = Finish_PipelineLayoutBuilder(builder->pipelineLayoutBuilder);
    PlBuilder_SetLayout(builder->pipelineBuilder, mat.pLayout);
    mat.pipeline = Finish_PipelineBuilder(builder->pipelineBuilder);
    return mat;
}

void Material_SetParameter(Material mat, uint32_t index, const void* value)
{
    assert(value);
    memcpy(mat.parameters[index].value, value, mat.parameters[index].size);
}

void Material_SetImageSlot(Material mat, uint32_t bind, ImageData imageData)
{
    VkDescriptorImageInfo imgInfo = {};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView = imageData.imageView;
	
	VkWriteDescriptorSet drawImageWrite = {};
	drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	drawImageWrite.pNext = NULL;
	
	drawImageWrite.dstBinding = bind;
	drawImageWrite.dstSet = mat.descSets[3];
	drawImageWrite.descriptorCount = 1;
	drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	drawImageWrite.pImageInfo = &imgInfo;

	vkUpdateDescriptorSets(mat.device, 1, &drawImageWrite, 0, NULL);
}

void Bind_Material(VkCommandBuffer cmnd, Material material)
{
    vkCmdBindPipeline(cmnd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
    vkCmdBindDescriptorSets(cmnd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pLayout, 0, 4, material.descSets, 0, NULL);
    for (int i = 0; i < material.parameterCount; i++)
    {
        vkCmdPushConstants(cmnd, material.pLayout, material.parameters[i].stage, material.parameters[i].offset, material.parameters[i].size, material.parameters[i].value);
    }
    
}