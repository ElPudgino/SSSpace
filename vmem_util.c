#include "vmem_util.h"
#include "engine_init.h"
#include "engine_utils.h"
#include "logger.h"

BufferInfo CreateBuffer(VmaAllocator alloc, size_t size, VmaMemoryUsage mem_property, VmaAllocationCreateFlagBits alloc_create, VkBufferUsageFlags buf_usage)
{
    BufferInfo n = {};

    VkBufferCreateInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bInfo.size = size;
    bInfo.usage = buf_usage;
    VmaAllocationCreateInfo cInfo = {};
    cInfo.usage = mem_property;
    cInfo.flags = alloc_create;
    int error = 0;
    if ((error = vmaCreateBuffer(alloc, &bInfo, &cInfo, &n.buffer, &n.allocation, &n.allocInfo)) != VK_SUCCESS)
    {
        LOG_INFO("!Failed to create buffer of size %lu, error: %d\n", size, error);
        return (BufferInfo){};
    }
    return n;
}

int Upload_Buffer(EngineState* engineState, void* arr, uint32_t size ,VkDeviceAddress* addr, BufferInfo* buffer)
{
    BufferInfo buf = CreateBuffer(engineState->allocator, size, 
        VMA_MEMORY_USAGE_CPU_ONLY,
        (VmaAllocationCreateFlagBits)0, 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    LOG_TEXT("Created buffers\n");

    VkBufferDeviceAddressInfo info = {.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = buf.buffer};
    *addr = vkGetBufferDeviceAddress(engineState->device, &info);
    *buffer = buf;
    LOG_TEXT("Got buffer device address\n");
    
    return Update_Buffer(engineState, arr, size, buffer);
}

int Update_Buffer(EngineState* engineState, void* arr, uint32_t size, BufferInfo* buffer)
{
    BufferInfo temp = CreateBuffer(engineState->allocator, size, 
        VMA_MEMORY_USAGE_CPU_ONLY, 
        (VmaAllocationCreateFlagBits)(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT ), 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    //LOG_TEXT("Created a temp buffer\n");

    // Map gpu memory with temp buffer to cpu array 

    if (vmaCopyMemoryToAllocation(engineState->allocator, arr, temp.allocation, 0, size) != VK_SUCCESS) return 1;
    //LOG_TEXT("Copied data\n");
    
    ImmediateCommand IC = Command_Immediate_Begin(engineState->device, engineState->commandsHandle.miscUpdateBuffer, engineState->queueHandles._Graphics);
    
    VkBufferCopy bufCopy = {};
    bufCopy.dstOffset = 0;
    bufCopy.srcOffset = 0;
    bufCopy.size = size;

    vkCmdCopyBuffer(IC.buffer, temp.buffer, buffer->buffer, 1, &bufCopy);

    //LOG_TEXT("copied ind buf\n");
    Command_Immediate_Complete(IC);
    //LOG_TEXT("submit imm cmnd\n");
    
    vmaDestroyBuffer(engineState->allocator, temp.buffer, temp.allocation);
    return 0;
}