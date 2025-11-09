#ifndef DESCRIPTORS_UTIL
#define DESCRIPTORS_UTIL

#include "libs.h"

typedef struct _DescriptorData
{
    VkDescriptorPool pool;
    VkDescriptorSet set;
    VkDescriptorSetLayout layout;
} DescriptorData;

typedef struct _DescriptorLayoutBuilder
{
    VkDescriptorSetLayoutBinding* bindings;
    VkDevice device;
    unsigned int bCount;   
    unsigned int cap;
} DescriptorLayoutBuilder;

typedef struct _DescriptorPoolSizes
{
    uint32_t sizesCount;
    VkDescriptorPoolSize* sizesPerSet;
} DescriptorPoolSizes;

DescriptorLayoutBuilder* Start_DescriptorLayoutBuilder(VkDevice device);

void DlBuilder_Add_DescriptorLayoutBinding(DescriptorLayoutBuilder* builder, unsigned int bindingNum, VkDescriptorType type, VkShaderStageFlags shaderStages);

VkDescriptorSetLayout Finish_DescriptorLayoutBuilder(DescriptorLayoutBuilder* builder, VkDescriptorSetLayoutCreateFlags flags);

VkDescriptorPool Create_DescriptorPool(VkDevice device, uint32_t maxSets, DescriptorPoolSizes sizes);

VkDescriptorSet Allocate_DescriptorSet(VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout);

#endif