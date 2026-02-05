#include "vmem_util.h"


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
        printf("!Failed to create buffer of size %lu, error: %d\n", size, error);
        return (BufferInfo){};
    }
    return n;
}
