#ifndef VMEM_UTILS
#define VMEM_UTILS

#include "libs.h"
#include "engine_init.h"

typedef struct _BufferInfo
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
} BufferInfo;

BufferInfo CreateBuffer(VmaAllocator alloc, size_t size, VmaMemoryUsage mem_property, VmaAllocationCreateFlagBits alloc_create, VkBufferUsageFlags buf_usage);

int Upload_Buffer(EngineState* engineState, void* arr, uint32_t size ,VkDeviceAddress* addr, BufferInfo* buffer);

#endif