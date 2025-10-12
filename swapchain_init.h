#ifndef SWAPCHAIN_INIT

#include "engine_init.h"

#define SWAPCHAIN_INIT

typedef struct _SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    uint32_t presentModesCount;
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR* presentModes;
} SwapChainSupportDetails;

int Cleanup_Swapchain(EngineState* engineState);

int Create_Swapchain(EngineState* engineState);

#endif