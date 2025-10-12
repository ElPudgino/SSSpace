#include <stdio.h>
#include "engine_utils.h"



int Create_Image(VmaAllocator allocator, ImageData* imageData, VkExtent3D extent)
{
    VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                              VK_IMAGE_USAGE_STORAGE_BIT |
                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


    VkImageCreateInfo iInfo = {};
    iInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    iInfo.pNext = NULL;
    iInfo.imageType = VK_IMAGE_TYPE_2D;
    iInfo.format = format;
    iInfo.extent = extent;
    iInfo.mipLevels = 1;
    iInfo.arrayLayers = 1;
    iInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    iInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    iInfo.usage = flags;


    imageData->imageExtent = extent;
    imageData->imageFormat = format;

    VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    vmaCreateImage(allocator, &iInfo, &allocInfo, &imageData->image, &imageData->allocation, NULL);

    VkImageViewCreateInfo ivInfo = {};
    ivInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ivInfo.pNext = NULL;
    ivInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ivInfo.image = imageData->image;
    ivInfo.format = format;
    ivInfo.subresourceRange.baseMipLevel = 0;
    ivInfo.subresourceRange.levelCount = 1;
    ivInfo.subresourceRange.baseArrayLayer = 0;
    ivInfo.subresourceRange.layerCount = 1;
    ivInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    return 0;
}   

VkImageSubresourceRange Get_ImageSubresourceRange(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange subRes = {};
    subRes.aspectMask = aspectMask;
    subRes.baseMipLevel = 0;
    subRes.levelCount = VK_REMAINING_MIP_LEVELS;
    subRes.baseArrayLayer = 0;
    subRes.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return subRes;
}

int Change_ImageFormat(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 imageBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    imageBarrier.pNext = NULL;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = currentLayout;
    imageBarrier.newLayout = newLayout;

    VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = Get_ImageSubresourceRange(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo depInfo = {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = NULL;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;
    vkCmdPipelineBarrier2(cmd, &depInfo);

    return 0;
}