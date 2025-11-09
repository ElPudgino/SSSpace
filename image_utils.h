#ifndef ENGINE_UTILS

#include "libs.h"

#define ENGINE_UTILS


typedef struct _ImageData
{
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
} ImageData;

int Create_Image(VkDevice device, VmaAllocator allocator, ImageData* imageData, VkExtent3D extent);

int Change_ImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

void Copy_ImageToImage(VkCommandBuffer cmnd, VkImage src, VkImage dst, VkExtent3D srcSize, VkExtent3D dstSize);

void Clear_Image(VkCommandBuffer cmnd, VkImage image, VkImageLayout layout, VkClearColorValue color);

#endif