#ifndef ENGINE_UTILs

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

int Create_Image(VmaAllocator allocator, ImageData* imageData, VkExtent3D extent);

int Change_ImageFormat(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

#endif