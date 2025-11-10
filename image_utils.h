#ifndef ENGINE_UTILS

#include "libs.h"

#define ENGINE_UTILS


typedef struct _ImageData
{
    VkImage image;
    VkImageView imageView;
    VkImageLayout layout;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
} ImageData;

int Create_Image(VkDevice device, VmaAllocator allocator, ImageData* imageData, VkExtent3D extent);

int Change_ImageLayout(VkCommandBuffer cmd, ImageData imageData, VkImageLayout newLayout);

void Copy_ImageToImage(VkCommandBuffer cmnd, ImageData src, ImageData dst);

void Clear_Image(VkCommandBuffer cmnd, ImageData imageData, VkClearColorValue color);

#endif