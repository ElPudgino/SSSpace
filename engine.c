#include <stdio.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

int Run_MainLoop(int* running)
{
    return 0;
}

int Init_SDL()
{
    int success = 1;
    success *= SDL_Init(SDL_INIT_VIDEO);
    success *= SDL_Init(SDL_INIT_AUDIO);
    return success;
}

int Create_MainSurface(VkInstance* instance, SDL_Window* window, VkSurfaceKHR* surface)
{
    return SDL_Vulkan_CreateSurface(window, *instance, NULL, surface);
}

int Cleanup()
{
    return 0;
}

int Init_Vulkan(VkInstance* instance, VkQueue* queue, VkDevice* device)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SSSpace";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo; 
    createInfo.ppEnabledExtensionNames = SDL_Vulkan_GetInstanceExtensions(&createInfo.enabledLayerCount);

    if (vkCreateInstance(&createInfo, NULL, instance) != VK_SUCCESS) return -printf("Failed to create VkInstance\n");

    // physical device
    uint32_t deviceCount = 0;
    VkPhysicalDevice physDevices[32] = {};

    VkPhysicalDevice physDevice;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, NULL);

    if (vkEnumeratePhysicalDevices(*instance, &deviceCount, physDevices) != VK_SUCCESS) return -printf("Failed to enumerate devices\n");
   
    if (deviceCount < 1) return -printf("Failed find devices\n");

    for (int ind = 0; ind < deviceCount; ind++)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physDevices[ind], &props);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physDevices[ind], &memoryProperties);

        printf("%s - available\n", props.deviceName);
    }

    physDevice = physDevices[0];

    // queue
    uint32_t queueFamilyCount = 0;
    VkQueueFamilyProperties queueFamilies[64] = {};
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, NULL);
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies);

    int graphicsQueueFamilyIndex = -1;
    for (int i = 0; i < queueFamilyCount; ++i) 
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // logical device
    VkPhysicalDeviceFeatures deviceFeatures = {};

    // Create the logical device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if (vkCreateDevice(physDevice, &deviceCreateInfo, NULL, device) != VK_SUCCESS) return -printf("Failed to create logical device");

    vkGetDeviceQueue(*device, graphicsQueueFamilyIndex, 0, queue);

    return 1;
}


typedef struct _SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    uint32_t presentModesCount;
    VkSurfaceFormatKHR formats[256];
    VkPresentModeKHR presentModes[10];
} SwapChainSupportDetails;

VkSurfaceFormatKHR Choose_SwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats, uint32_t count) {
    // Prefer SRGB color space with 8-bit normalized RGBA
    for (int ind = 0; ind < count; ind++) {
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
    for (int ind = 0; ind < count; ind++) {
        if (availablePresentModes[ind] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[ind];
        }
    }
    // Fallback to FIFO (guaranteed to be available)
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Choose_SwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, SDL_Window* window) {
    // If current extent is defined, use it
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {
        // Otherwise choose based on window size
        uint32_t width, height;
        SDL_GetWindowSizeInPixels(window, &width, &height);
        
        VkExtent2D actualExtent = {
            width,
            height
        };
        
        // Clamp to supported min/max values
        actualExtent.width = max(capabilities->minImageExtent.width, 
            min(capabilities->maxImageExtent.width, actualExtent.width));
        actualExtent.height = max(capabilities->minImageExtent.height, 
            min(capabilities->maxImageExtent.height, actualExtent.height));
        
        return actualExtent;
    }
}

SwapChainSupportDetails Get_SwapchainDetails(VkPhysicalDevice* device, VkSurfaceKHR* surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, *surface, &details.capabilities);
    
    // Get supported surface formats
    uint32_t formatCount;
    
    vkGetPhysicalDeviceSurfaceFormatsKHR(*device, *surface, &formatCount, NULL);
    if (formatCount != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(*device, *surface, &formatCount, details.formats);
    }
    details.formatCount = formatCount;
    // Get supported present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*device, *surface, &presentModeCount, NULL);
    if (presentModeCount != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(*device, *surface, &presentModeCount, details.presentModes);
    }
    details.presentModesCount = presentModeCount;

    return details;
}

int Create_Swapchain(VkPhysicalDevice* device, VkSurfaceKHR* surface, VkSwapchainKHR* swapchain, SDL_Window* window)
{
    SwapChainSupportDetails swapChainSupport = Get_SwapchainDetails(device, surface);
    
    VkSurfaceFormatKHR surfaceFormat = Choose_SwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatCount);
    VkPresentModeKHR presentMode = Choose_SwapPresentMode(swapChainSupport.presentModes, swapChainSupport.presentModesCount);
    VkExtent2D extent = Choose_SwapExtent(&swapChainSupport.capabilities, window);
}

int main(int argc, char** argv)
{
    VkInstance instance;
    VkDevice device;
    VkQueue queue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    
    SDL_Window* mainWindow;

    int running = 1;

    
    Init_SDL();
    mainWindow = SDL_CreateWindow("SSSpace", 1200, 800, SDL_WINDOW_VULKAN);

    Init_Vulkan(&instance, &queue, &device);
    Create_MainSurface(&instance, mainWindow, &surface);

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) // process all input before main update
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
                break;
            }
        }

        Run_MainLoop(&running);
    }

    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);

}
