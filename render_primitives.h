#ifndef RENDER_PRIMITIVES
#define RENDER_PRIMITIVES

#include "libs.h"

typedef struct _MaterialParameter
{
    uint32_t size;
    uint32_t offset;
    VkShaderStageFlags stage;
    void* value;
} MaterialParameter;

typedef struct _Material
{
    VkDevice device;
    VkPipeline pipeline;
    VkDescriptorPool ownPool;
    VkDescriptorSetLayout ownLayout;
    VkDescriptorSet descSets[4]; // last set is material own set 
    VkPipelineLayout pLayout;
    MaterialParameter* parameters;
    uint32_t parameterCount;
} Material;


#endif