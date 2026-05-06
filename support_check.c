#include <stdio.h>
#include <math.h>
#include "engine_init.h"
#include "vk_mem_alloc.h"


// Select queue families from available on startup
// Return 0 if all 5 queueFamilies were found 1 otherwise
int Select_QueueFamilies(EngineState* engineState)//QueueFamIndices* Selected, VkPhysicalDevice* device, VkSurfaceKHR* surface)
{
    assert(engineState);
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(engineState->physicalDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)calloc(queueFamilyCount, sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(engineState->physicalDevice, &queueFamilyCount, queueFamilies);

    int completion[5] = {};
    int QueueFamilyIndex = -1;
    for (int i = 0; i < queueFamilyCount; ++i) // Select the first suitable queueFamily
    {
        if (!completion[0] && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            engineState->queueFamIndices._Graphics = i;
            completion[0] = 1;
        }
        if (!completion[1] && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) 
        {
            engineState->queueFamIndices._Compute = i;
            completion[1] = 1;
        }
        if (!completion[2] && queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) 
        {
            engineState->queueFamIndices._Sparse = i;
            completion[2] = 1;
        }
        if (!completion[3] && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) 
        {
            engineState->queueFamIndices._Transfer = i;
            completion[3] = 1;
        }
        VkBool32 presentSupport = 0;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(engineState->physicalDevice, i, engineState->surface, &presentSupport) != VK_SUCCESS) 
        {
            free(queueFamilies);
            return -printf("Failed to get physical device surface support details\n");
        }
        if (!completion[4] && presentSupport) 
        {
            engineState->queueFamIndices._Present = i;
            completion[4] = 1;
        }
    }

    free(queueFamilies);

    if (!completion[1] && completion[0])
    {
        printf("No supported compute queueFamily found, using graphics queueFamily instead\n");
        completion[1] = 1;
        engineState->queueFamIndices._Compute = engineState->queueFamIndices._Graphics;
    }
    if (!completion[2])
    {
        printf("No supported sparse queueFamily found, using graphics queueFamily instead\n");
        completion[2] = 1;
        engineState->queueFamIndices._Sparse = engineState->queueFamIndices._Graphics;
    }
    if (!completion[3] && completion[0])
    {
        printf("No supported transfer queueFamily found, using graphics queueFamily instead\n");
        completion[3] = 1;
        engineState->queueFamIndices._Transfer = engineState->queueFamIndices._Graphics;
    } 

    return !(completion[0] && completion[1] && completion[2] && completion[3] && completion[4]);
}

static int extCount = 4;
static char const* extensions[4];
char const** Get_DeviceExtensions()
{
    extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    extensions[1] = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
    extensions[2] = VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME;
    extensions[3] = VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME;
    return extensions;
}

int Get_ExtensionCount()
{
    return extCount;
}

int Check_ExtensionSupport(EngineState* engineState)
{
    Get_DeviceExtensions();
    uint32_t DextCount = 0;
    VkExtensionProperties* props = NULL;
    vkEnumerateDeviceExtensionProperties(engineState->physicalDevice, NULL, &DextCount, NULL);
    props = (VkExtensionProperties*)calloc(DextCount, sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(engineState->physicalDevice, NULL, &DextCount, props);

    int found = 0;
    for (int i = 0; i < extCount; i++)
    {
        found = 0;
        for (int e = 0; e < DextCount; e++)
        {
            //printf("Found extension: %s\n", props[e].extensionName);
            if (!strcmp(props[e].extensionName,extensions[i])) {found = 1; break;}
        }
        if (!found)
        {
            printf("!!Failed to find extension: %s\n", extensions[i]);
            return 1;
        }
    }
    free(props);
    return 0;
}

