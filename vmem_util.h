#ifndef VMEM_UTILS
#define VMEM_UTILS

#include "libs.h"
#include "engine_init.h"

BufferInfo CreateBuffer(VmaAllocator alloc, size_t size, VmaMemoryUsage mem_property, VmaAllocationCreateFlagBits alloc_create, VkBufferUsageFlags buf_usage);

int Upload_Buffer(EngineState* engineState, void* arr, uint32_t size ,VkDeviceAddress* addr, BufferInfo* buffer);

int Update_Buffer(EngineState* engineState, void* arr, uint32_t size, BufferInfo* buffer);

#endif