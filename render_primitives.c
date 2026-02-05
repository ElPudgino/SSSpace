#include "render_primitives.h"

#define PUSH_CONSTANTS_MAX_SIZE 128

void _MatBuilder_Increase_PoolSize(MaterialBuilder* builder, VkDescriptorType descType)
{
    assert(builder);
    if (builder->poolSizes.sizeCap == 0)
    {
        builder->poolSizes.sizesPerSet = (VkDescriptorPoolSize*)calloc(1, sizeof(VkDescriptorPoolSize));
        builder->poolSizes.sizeCap = 1;
        builder->poolSizes.sizesCount = 1;
        builder->poolSizes.sizesPerSet[0] = (VkDescriptorPoolSize){.type = descType, .descriptorCount = 1};
        return;
    }
    for (int i = 0; i < builder->poolSizes.sizesCount; i++)
    {
        if (builder->poolSizes.sizesPerSet[i].type == descType)
        {
            builder->poolSizes.sizesPerSet[i].descriptorCount++;
            return;
        }
    }
    if (builder->poolSizes.sizesCount = builder->poolSizes.sizeCap)
    {
        builder->poolSizes.sizesPerSet = (VkDescriptorPoolSize*)realloc(builder->poolSizes.sizesPerSet, sizeof(VkDescriptorPoolSize)*builder->poolSizes.sizeCap*2);
        builder->poolSizes.sizeCap *= 2;
    }
    builder->poolSizes.sizesPerSet[builder->poolSizes.sizesCount] = (VkDescriptorPoolSize){.type = descType, .descriptorCount = 1};
}

MaterialBuilder* Start_MaterialBuilder(VkDevice device)
{
    MaterialBuilder* builder = (MaterialBuilder*)calloc(1, sizeof(MaterialBuilder));
    builder->poolSizes.sizesCount = 0;
    builder->poolSizes.sizeCap = 0;

    builder->matParams = (MaterialParameter*)calloc(1, sizeof(MaterialParameter));

    builder->descLayoutBuilder = Start_DescriptorLayoutBuilder(device);
    builder->pipelineLayoutBuilder = Start_PipelineLayoutBuilder(device);
    builder->pipelineBuilder = Start_PipelineBuilder(device);

    PlBuilder_Set_ColorFormat(builder->pipelineBuilder, MAIN_RENDER_IMAGE_FORMAT);
    //PlBuilder_Set_DepthFormat(builder->pipelineBuilder, VK_FORMAT_D32_SFLOAT_S8_UINT);
    //PlBuilder_Set_StencilFormat(builder->pipelineBuilder, VK_FORMAT_D32_SFLOAT_S8_UINT);

    builder->device = device;

    //MatBuilder_AddParameter(builder, sizeof(MeshParameter), VK_SHADER_STAGE_VERTEX_BIT);

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

Material* Finish_MaterialBuilder(MaterialBuilder* builder)
{
    assert(builder);
    Material* mat = (Material*)calloc(1, sizeof(Material));
    mat->parameterCount = builder->matParamsCount;
    mat->parameters = builder->matParams;
    mat->device = builder->device;
    // Order of these commands is important
    mat->ownLayout = Finish_DescriptorLayoutBuilder(builder->descLayoutBuilder, 0);
    mat->ownPool = Create_DescriptorPool(builder->device, 1, builder->poolSizes);

    PllBuilder_Add_DescriptorSet(builder->pipelineLayoutBuilder, mat->ownLayout);

    mat->descSets[3] = Allocate_DescriptorSet(builder->device, mat->ownPool, mat->ownLayout);
    mat->pLayout = Finish_PipelineLayoutBuilder(builder->pipelineLayoutBuilder);
    PlBuilder_SetLayout(builder->pipelineBuilder, mat->pLayout);
    mat->pipeline = Finish_PipelineBuilder(builder->pipelineBuilder);
    return mat;
}

void Material_SetParameter(Material* mat, uint32_t index, const void* value)
{
    assert(value);
    memcpy(mat->parameters[index].value, value, mat->parameters[index].size);
}

void Material_SetImageSlot(Material* mat, uint32_t bind, ImageData imageData)
{
    VkDescriptorImageInfo imgInfo = {};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView = imageData.imageView;
	
	VkWriteDescriptorSet drawImageWrite = {};
	drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	drawImageWrite.pNext = NULL;
	
	drawImageWrite.dstBinding = bind;
	drawImageWrite.dstSet = mat->descSets[MATERIAL_DESC_SET_COUNT-1];
	drawImageWrite.descriptorCount = 1;
	drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	drawImageWrite.pImageInfo = &imgInfo;

	vkUpdateDescriptorSets(mat->device, 1, &drawImageWrite, 0, NULL);
}

void Bind_Material(VkCommandBuffer cmnd, Material* material)
{
    assert(material->pLayout);
    assert(material->descSets);
    
    vkCmdBindPipeline(cmnd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);
    // todo: change this to bind correctly when materials have different amounts of descsets
    vkCmdBindDescriptorSets(cmnd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pLayout, 0, 1, &material->descSets[MATERIAL_DESC_SET_COUNT-1], 0, NULL);
    for (int i = 0; i < material->parameterCount; i++)
    {
        vkCmdPushConstants(cmnd, material->pLayout, material->parameters[i].stage, material->parameters[i].offset, material->parameters[i].size, material->parameters[i].value);
    }
}

void Destroy_Material(Material* mat)
{
    vkDeviceWaitIdle(mat->device); // ! Could cause performance issues if materials are destroyed frequently
                                   // Shouldnt be an issue if materials are created/destroyed on app startup/closing
    vkDestroyPipeline(mat->device, mat->pipeline, NULL);
    vkDestroyPipelineLayout(mat->device, mat->pLayout, NULL);
    vkResetDescriptorPool(mat->device, mat->ownPool, 0);
    vkDestroyDescriptorPool(mat->device, mat->ownPool, NULL);
    vkDestroyDescriptorSetLayout(mat->device, mat->ownLayout, NULL);
    if (mat->parameters)
    {
        free(mat->parameters);
    }
    free(mat);
}