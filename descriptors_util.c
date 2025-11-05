#include "engine_init.h"
#include <stdio.h>
#include "descriptors_util.h"

DescriptorLayoutBuilder* Start_DescriptorLayoutBuilder(EngineState* engineState)
{
    DescriptorLayoutBuilder* res = (DescriptorLayoutBuilder*)calloc(1, sizeof(DescriptorLayoutBuilder));
    res->bindings = (VkDescriptorSetLayoutBinding*)calloc(4, sizeof(VkDescriptorSetLayoutBinding));
    res->cap = 4;
    res->device = engineState->device;
    return res;
}

void DlBuilder_Add_DescriptorLayoutBinding(DescriptorLayoutBuilder* builder, unsigned int bindingNum, VkDescriptorType type, VkShaderStageFlags shaderStages)
{
    if (builder->bCount == builder->cap) builder->bindings = (VkDescriptorSetLayoutBinding*)realloc(builder->bindings, builder->cap*2*sizeof(VkDescriptorSetLayoutBinding));

    builder->bindings[builder->bCount] = (VkDescriptorSetLayoutBinding){};
    builder->bindings[builder->bCount].binding = bindingNum;
    builder->bindings[builder->bCount].descriptorCount = 1;
    builder->bindings[builder->bCount].descriptorType = type;
    builder->bindings[builder->bCount].stageFlags = shaderStages;
    
    builder->bCount++;
}

VkDescriptorSetLayout Finish_DescriptorLayoutBuilder(DescriptorLayoutBuilder* builder, VkDescriptorSetLayoutCreateFlags flags)
{
    VkDescriptorSetLayoutCreateInfo cInfo = {};
    cInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    cInfo.pNext = NULL;
    cInfo.bindingCount = builder->bCount;
    cInfo.pBindings = builder->bindings;
    cInfo.flags = flags;

    VkDescriptorSetLayout set;
    if (vkCreateDescriptorSetLayout(builder->device, &cInfo, NULL, &set) != VK_SUCCESS) {printf("Failed to create descriptor layout\n"); free(builder->bindings); free(builder); return NULL;} 
    free(builder->bindings);
    free(builder);
    return set;
}
/* 
* All Descriptor sets in a pool are the same
*/
VkDescriptorPool Create_DescriptorPool(EngineState* engineState, uint32_t maxSets, DescriptorPoolSizes sizes)
{
    VkDescriptorPoolCreateInfo cInfo = {};
    cInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    cInfo.maxSets = maxSets;
    cInfo.poolSizeCount = sizes.sizesCount;
    cInfo.pPoolSizes = sizes.sizesPerSet;
    for (int i = 0; i < sizes.sizesCount; i++)
    {
        sizes.sizesPerSet[i].descriptorCount *= maxSets;
    }

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(engineState->device, &cInfo, NULL, &pool)) {printf("Failed to create descriptor pool, max sets: %u\n", maxSets); return NULL;}
    return pool;
}

VkDescriptorSet Allocate_DescriptorSet(EngineState* engineState, VkDescriptorPool pool, VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    allocInfo.pNext = NULL;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet ds;
    if (vkAllocateDescriptorSets(engineState->device, &allocInfo, &ds)) {printf("Failed to allocate descriptor set\n"); return NULL;}
    return ds;
}