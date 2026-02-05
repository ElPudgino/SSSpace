#ifndef VMEM_UTILS
#define VMEM_UTILS

#include "libs.h"

typedef struct _BufferInfo
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
} BufferInfo;

BufferInfo CreateBuffer(VmaAllocator alloc, size_t size, VmaMemoryUsage mem_property, VmaAllocationCreateFlagBits alloc_create, VkBufferUsageFlags buf_usage);

#endif