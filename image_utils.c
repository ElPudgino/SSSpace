#include <stdio.h>
#include "image_utils.h"

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

int Create_Image(VkDevice device, VmaAllocator allocator, ImageData* imageData, VkExtent3D extent)
{
    assert(imageData);
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
    imageData->layout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (vmaCreateImage(allocator, &iInfo, &allocInfo, &imageData->image, &imageData->allocation, NULL) != VK_SUCCESS) return -printf("!!Failed to allocate image\n");

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

    if (vkCreateImageView(device, &ivInfo, NULL, &imageData->imageView) != VK_SUCCESS) return -printf("!!Failed to create image view for allocated image\n");


    return 0;
}   

void Clear_Image(VkCommandBuffer cmnd, ImageData imageData, VkClearColorValue color)
{
    VkImageSubresourceRange range = Get_ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(cmnd, imageData.image, imageData.layout, &color, 1, &range);
}

void Copy_ImageToImage(VkCommandBuffer cmnd, ImageData src, ImageData dst)
{
    VkImageBlit2 blitInfo = {};
    blitInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitInfo.dstSubresource.baseArrayLayer = 0;
    blitInfo.dstSubresource.layerCount = 1;
    blitInfo.dstSubresource.mipLevel = 0;

    blitInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitInfo.srcSubresource.baseArrayLayer = 0;
    blitInfo.srcSubresource.layerCount = 1;
    blitInfo.srcSubresource.mipLevel = 0;
    
    blitInfo.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
    // Setting Top Right corner of the copy area
    blitInfo.srcOffsets[1].x = src.imageExtent.width;
    blitInfo.srcOffsets[1].y = src.imageExtent.height;
    blitInfo.srcOffsets[1].z = src.imageExtent.depth;

    blitInfo.dstOffsets[1].x = dst.imageExtent.width;
    blitInfo.dstOffsets[1].y = dst.imageExtent.height;
    blitInfo.dstOffsets[1].z = dst.imageExtent.depth;

    VkBlitImageInfo2 iInfo = {};
    iInfo.regionCount = 1;
    iInfo.pRegions = &blitInfo;
    iInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    iInfo.srcImage = src.image;
    iInfo.dstImage = dst.image;
    iInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    iInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    iInfo.filter = VK_FILTER_LINEAR;
    vkCmdBlitImage2(cmnd, &iInfo);
}

int Change_ImageLayout(VkCommandBuffer cmd, ImageData imageData, VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 imageBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    imageBarrier.pNext = NULL;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = imageData.layout;
    imageBarrier.newLayout = newLayout;

    VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = Get_ImageSubresourceRange(aspectMask);
    imageBarrier.image = imageData.image;

    VkDependencyInfo depInfo = {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = NULL;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;
    vkCmdPipelineBarrier2(cmd, &depInfo);

    return 0;
}