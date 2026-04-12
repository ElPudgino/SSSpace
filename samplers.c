#include "image_utils.h"
#include "engine_init.h"

VkSampler pixelSampler;

void Create_PixelSampler(EngineState* engineState)
{
    VkSamplerCreateInfo cinfo = {};
    cinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    cinfo.minFilter = VK_FILTER_NEAREST;
    cinfo.magFilter = VK_FILTER_NEAREST;
    cinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    cinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    cinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

    vkCreateSampler(engineState->device, &cinfo, NULL, &pixelSampler);
}

int Create_Samplers(EngineState* engineState)
{
    Create_PixelSampler(engineState);
    return 0;
}

int Destroy_Samplers(EngineState* engineState)
{
    vkDestroySampler(engineState->device, pixelSampler, NULL);
    return 0;
}

VkSampler Get_PixelSampler()
{
    return pixelSampler;
}