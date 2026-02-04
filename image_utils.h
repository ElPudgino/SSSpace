#ifndef IMAGE_UTILS

#include "libs.h"

#define IMAGE_UTILS

/*! @brief Stores all info related to an image
* Avoid incompletely initializing this structs data
* @param image Image itself
* @param imageView ImageView to the image
* @param layout Current layout of the image
* @param allocation If image was allocated manually stores allocation, null otherwise
* @param imageExtent Image size
* @param imageFormat Format of the image
*/
typedef struct _ImageData
{
    VkImage image;
    VkImageView imageView;
    VkImageLayout layout;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
} ImageData;

/*! @brief Allocates an image and fills imageData
* Image must be destroyed later
* @param device Device on which image is allocated
* @param allocator VMA
* @param imageData Pointer to ImageData which will be initialized
* @param extent Image size
*/
int Create_Image(VkDevice device, VmaAllocator allocator, ImageData* imageData, VkExtent3D extent);

/*! @brief Record a command to change image layout
* @param cmnd Used command buffer
* @param imageData Image information
* @param newLayout Target layout
*/
int Change_ImageLayout(VkCommandBuffer cmnd, ImageData* imageData, VkImageLayout newLayout);

/*! @brief Copy image to image with rescaling
* @param cmnd Used command buffer
* @param src Copy source
* @param dst Copy destination
*/
void Copy_ImageToImage(VkCommandBuffer cmnd, ImageData src, ImageData dst);

void Clear_Image(VkCommandBuffer cmnd, ImageData imageData, VkClearColorValue color);

#endif