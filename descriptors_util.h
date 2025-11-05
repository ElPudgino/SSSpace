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

#endif