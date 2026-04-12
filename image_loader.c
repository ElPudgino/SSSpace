#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#include "engine_init.h"
#include "logger.h"
#include "render_primitives.h"
#include "engine_utils.h"
#include "texture_table.h"
#include "dirent.h"

int Load_ImageFromFile(EngineState* engineState, char* file, ImageData** img)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) {LOG_INFO("!Failed to load texture from: %s\n", file); return 1;}

    VkDeviceSize imgSize = texWidth * texHeight * 4;

    BufferInfo temp = CreateBuffer(engineState->allocator, (size_t)imgSize, 
        VMA_MEMORY_USAGE_CPU_ONLY, 
        (VmaAllocationCreateFlagBits)(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT ), 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    
    if (VK_SUCCESS != vmaCopyMemoryToAllocation(engineState->allocator, (void*)pixels, temp.allocation, 0, imgSize))
    {
        LOG_INFO("Failed to copy texture to temp buffer: %s\n", file);
        return 1;
    }

    stbi_image_free(pixels);

    ImageData* image = (ImageData*)calloc(1, sizeof(ImageData));

    VkExtent3D ext = {.width = (uint32_t)texWidth, .height = (uint32_t)texHeight, .depth = 1};
    if (Create_ImageGeneric(engineState->device, engineState->allocator, image, ext, 
        VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT))
    {
        LOG_INFO("Failed to create image for texture: %s\n", file);
        return 1;
    }

    ImmediateCommand ic = Command_Immediate_Begin(engineState->device, engineState->commandsHandle.miscUpdateBuffer, engineState->queueHandles._Graphics);
    Change_ImageLayout(ic.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy copyRegion = {};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;

	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageExtent = ext;

    vkCmdCopyBufferToImage(ic.buffer, temp.buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    Change_ImageLayout(ic.buffer, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    Command_Immediate_Complete(ic);
    
    vmaDestroyBuffer(engineState->allocator, temp.buffer, temp.allocation);

    *img = image;

    LOG_INFO("Loaded tex: %s\n", file);
    return 0;
}

int Load_GameTextures(EngineState* engineState)
{
    if (Init_TextureTable()) return LOG_TEXT("!Failed to init texture table\n"), 1;

    char* fileNameBuf = (char*)calloc(400, sizeof(char));

    DIR* dr;
    struct dirent* de;

    dr = opendir("assets/textures");

    if (!dr) return LOG_TEXT("!Failed to open textures directory\n"), free(fileNameBuf), 1;

    ImageData* img = NULL;
    while (de = readdir(dr))
    {
        LOG_INFO("reading dir: %s\n",de->d_name);
        if (de->d_name[0] == '.') continue;
        snprintf(fileNameBuf, 320, "assets/textures/%s", de->d_name);
        if (Load_ImageFromFile(engineState, fileNameBuf, &img)) {LOG_INFO("!Failed to load texture: %s\n", fileNameBuf); free(fileNameBuf); return 1;}
        
        TextureTable_Set_Texture(de->d_name, img);
    }
    closedir(dr);
    free(fileNameBuf);
    return 0;
}

int Unload_Textures(EngineState* engineState)
{
    CleanUp_TextureTable();
    return 0;
}