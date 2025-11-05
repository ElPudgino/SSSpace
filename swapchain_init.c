#include <stdio.h>
#include <math.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include "swapchain_init.h"
#include <assert.h>


int Get_SwapchainDetails(VkPhysicalDevice* device, VkSurfaceKHR* surface, SwapChainSupportDetails* dets)
{
    SwapChainSupportDetails details = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, *surface, &details.capabilities);

    // Get supported surface formats
    uint32_t formatCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(*device, *surface, &formatCount, NULL) != VK_SUCCESS) return -printf("Failed to get Physical Device Surface Formats count\n");
    details.formats = (VkSurfaceFormatKHR*)calloc(formatCount, sizeof(VkSurfaceFormatKHR));
    if (formatCount != 0)
    {
        int res = vkGetPhysicalDeviceSurfaceFormatsKHR(*device, *surface, &formatCount, details.formats);
        if (res != VK_SUCCESS) return -printf("!!Failed to get Physical Device Surface Formats\n");
    }
    details.formatCount = formatCount;


    // Get supported present modes
    uint32_t presentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(*device, *surface, &presentModeCount, NULL)) return -printf("Failed to get Physical Device Surface Present Modes count\n");
    details.presentModes = (VkPresentModeKHR*)calloc(presentModeCount, sizeof(VkPresentModeKHR));
    if (presentModeCount != 0)
    {
        int res = vkGetPhysicalDeviceSurfacePresentModesKHR(*device, *surface, &presentModeCount, details.presentModes);
        if (res != VK_SUCCESS) return -printf("!!Failed to get Physical Device Surface Present Modes\n");
    }
    details.presentModesCount = presentModeCount;

    *dets = details;

    return VK_SUCCESS;
}

VkSurfaceFormatKHR Choose_SwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats, uint32_t count) {
    // Prefer SRGB color space with 8-bit normalized RGBA
    for (int ind = 0; ind < count; ind++)
    {
        if (availableFormats[ind].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormats[ind].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[ind];
        }
    }
    // Fallback to first available format
    return availableFormats[0];
}

VkPresentModeKHR Choose_SwapPresentMode(const VkPresentModeKHR* availablePresentModes, uint32_t count) {
    // Prefer mailbox (triple buffering) for low latency
    for (int ind = 0; ind < count; ind++) 
    {
        if (availablePresentModes[ind] == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentModes[ind];
        }
    }
    // Fallback to FIFO (guaranteed to be available)
    return VK_PRESENT_MODE_FIFO_KHR;
}

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

VkExtent2D Choose_SwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, SDL_Window* window) {
    // If current extent is defined, use it
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {
        // Otherwise choose based on window size
        int width, height;
        SDL_GetWindowSizeInPixels(window, &width, &height);

        VkExtent2D actualExtent = {
            (unsigned int)width,
            (unsigned int)height
        };

        // Clamp to supported min/max values
        actualExtent.width = MAX(capabilities->minImageExtent.width,
            MIN(capabilities->maxImageExtent.width, actualExtent.width));
        actualExtent.height = MAX(capabilities->minImageExtent.height,
            MIN(capabilities->maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

// If creation fails then swapchainState is NULL, else it must be freed when shutting down
int Create_Swapchain(EngineState* engineState)
{
    SwapChainSupportDetails swapChainSupport = {};
    if (Get_SwapchainDetails(&engineState->physicalDevice, &engineState->surface, &swapChainSupport) != VK_SUCCESS) return -printf("Failed to get swapchain support details\n");

    printf("Got swapchain details\n");
    VkSurfaceFormatKHR surfaceFormat = Choose_SwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatCount);
    printf("Chose sc format\n");
    VkPresentModeKHR presentMode = Choose_SwapPresentMode(swapChainSupport.presentModes, swapChainSupport.presentModesCount);
    printf("Chose sc present mode\n");
    VkExtent2D extent = Choose_SwapExtent(&swapChainSupport.capabilities, engineState->window);

    engineState->swapchainState.extent = (VkExtent3D){extent.width,extent.height,1};

    free(swapChainSupport.formats);
    free(swapChainSupport.presentModes);

    printf("Chose swapchain params\n");
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = engineState->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // Always 1 unless stereoscopic 3D
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_GrPr[] = {engineState->queueFamIndices._Graphics, engineState->queueFamIndices._Present};
    if (queue_GrPr[0] != queue_GrPr[1]) {
        // Concurrent mode - images can be used across queue families
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queue_GrPr;
    } else {
        // Exclusive mode - better performance
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
    }
    // Don't transform images
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    // Don't blend with other windows
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; // Don't care about obscured pixels
    createInfo.oldSwapchain = VK_NULL_HANDLE; // Used for swapchain recreation

    if (engineState->swapchainState.created)
    {
        vkDeviceWaitIdle(engineState->device);
        Cleanup_Swapchain(engineState);
    }
    
    
    printf("Allocated swapchain\n");

    if (vkCreateSwapchainKHR(engineState->device, &createInfo, NULL, &engineState->swapchainState.swapchain) != VK_SUCCESS)
    {
        return -printf("!!Failed to Create Swapchain\n");
    }

    if (vkGetSwapchainImagesKHR(engineState->device, engineState->swapchainState.swapchain, &imageCount, NULL) != VK_SUCCESS)
    {
        return -printf("!!Failed to get swapchain images amount\n");
    }

    engineState->swapchainState.images = (VkImage*)calloc(imageCount, sizeof(VkImage));

    if (vkGetSwapchainImagesKHR(engineState->device, engineState->swapchainState.swapchain, &imageCount, engineState->swapchainState.images) != VK_SUCCESS)
    {
        free(engineState->swapchainState.images);
        return -printf("!!Failed to get swapchain images\n");
    }


    engineState->swapchainState.imageViews = (VkImageView*)calloc(imageCount, sizeof(VkImageView));


    for (size_t i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = engineState->swapchainState.images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        createInfo.flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Default mapping of color channels
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Image subresource range
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(engineState->device, &createInfo, NULL, &engineState->swapchainState.imageViews[i]) != VK_SUCCESS)
        {
            free(engineState->swapchainState.images);
            free(engineState->swapchainState.imageViews);
            return -printf("!!Failed to create image view\n");
        }
    }

    engineState->swapchainState.imageCount = imageCount;
    engineState->swapchainState.created = 1;
    return 0;
}

int Cleanup_Swapchain(EngineState* engineState)
{
    assert(engineState);

    if (!engineState->swapchainState.created)
    {
        printf("!Swapchain was NULL when cleaning up\n");
        return 0;
    }

    uint32_t imageCount = engineState->swapchainState.imageCount;


    for (int i = 0; i < imageCount; i++)
    {
        vkDestroyImageView(engineState->device, engineState->swapchainState.imageViews[i], NULL);
    }
    vkDestroySwapchainKHR(engineState->device, engineState->swapchainState.swapchain, NULL);

    free(engineState->swapchainState.imageViews);
    free(engineState->swapchainState.images);
    return 0;
}
