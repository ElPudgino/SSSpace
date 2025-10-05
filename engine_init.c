#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>
#include "swapchain_init.h"


int Create_VulkanInstance(EngineState* engineState)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SSSpace";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {};
    uint32_t extCount = 0;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo; 
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledExtensionNames = SDL_Vulkan_GetInstanceExtensions(&extCount);
    createInfo.enabledExtensionCount = extCount;
    
    
    int res = 0;
    if ((res = vkCreateInstance(&createInfo, NULL, &engineState->instance) != VK_SUCCESS)) return -printf("Failed to create VkInstance: %d\n", res);

    return 0;
}

int Get_PhysicalDevice(EngineState* engineState)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(engineState->instance, &deviceCount, NULL);
    VkPhysicalDevice* physDevices = (VkPhysicalDevice*)calloc(deviceCount, sizeof(VkPhysicalDevice));

    int res = 0;
    if ((res = vkEnumeratePhysicalDevices(engineState->instance, &deviceCount, physDevices)) != VK_SUCCESS) 
    {
        printf("Failed to enumerate devices, error code: %d\n", res);
        free(physDevices);
        return -1;
    }
    
    if (deviceCount < 1) 
    {
        printf("Failed find devices\n");
        free(physDevices);
        return -2;
    }

    VkPhysicalDeviceProperties props = {};
    for (int ind = 0; ind < deviceCount; ind++)
    {     
        vkGetPhysicalDeviceProperties(physDevices[ind], &props);

        printf("%s - device available\n", props.deviceName);
    }

    engineState->physicalDevice = physDevices[0];

    vkGetPhysicalDeviceProperties(physDevices[0], &props);
    printf("%s - device SELECTED\n", props.deviceName);

    free(physDevices);
    return res;
}

int Create_MainSurface(EngineState* engineState)
{
    return !SDL_Vulkan_CreateSurface(engineState->window, engineState->instance, NULL, &engineState->surface);
}

int Create_LogicalDevice(EngineState* engineState)
{
    uint32_t families[5] = {engineState->queueFamIndices._Graphics,
                       engineState->queueFamIndices._Compute, 
                       engineState->queueFamIndices._Sparse,
                       engineState->queueFamIndices._Transfer, 
                       engineState->queueFamIndices._Present};

    int uniqueFcount = 0;
    int f = 0;
    for (int i = 0; i < 5; i++) // Make an array of unique queuefamily indices
    {
        f = 1;
        for (int j = i - 1; j >= 0; j--)
        {
            if (families[i] == families[j]) 
            {
                f = 0;
                families[i] = (uint32_t)-1;
                break;
            }
        }
        uniqueFcount += f;
    }
    // For each unique queueFamily create a queue
    VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)calloc(uniqueFcount, sizeof(VkDeviceQueueCreateInfo));
    f = 0;
    float queuePriority = 1.0f;
    for (int i = 0; i < 5; i++)
    {
        if (families[i] != (uint32_t)-1) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = families[i];
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfo.flags = 0;

            queueCreateInfos[f] = queueCreateInfo;
            f++;
        }
    }
    // Actual creation of logical device
    VkPhysicalDeviceFeatures deviceFeatures = {}; 

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = uniqueFcount;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    uint32_t extensionCount = 1;
    char const** extensions = (char const**)calloc(extensionCount, sizeof(char const**));
    extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    // Add required extensions (like swapchain)
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;


    int res = 0;
    if ((res = vkCreateDevice(engineState->physicalDevice, &createInfo, NULL, &engineState->device)) != VK_SUCCESS) printf("Failed to create logical device, error code: %d\n", res);
   
    free(extensions);
    free(queueCreateInfos);
    return res;
}

int Get_QueueHandles(EngineState* engineState)
{
    engineState->queueHandles._Compute = (VkQueue*)calloc(1, sizeof(VkQueue));
    engineState->queueHandles._Graphics = (VkQueue*)calloc(1, sizeof(VkQueue));
    engineState->queueHandles._Sparse = (VkQueue*)calloc(1, sizeof(VkQueue));
    engineState->queueHandles._Transfer = (VkQueue*)calloc(1, sizeof(VkQueue));
    engineState->queueHandles._Present = (VkQueue*)calloc(1, sizeof(VkQueue));

    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Compute, 0, engineState->queueHandles._Compute);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Graphics, 0, engineState->queueHandles._Graphics);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Sparse, 0, engineState->queueHandles._Sparse);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Transfer, 0, engineState->queueHandles._Transfer);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Present, 0, engineState->queueHandles._Present);
    return 0;
}

int Create_MainWindow(EngineState* engineState)
{
    engineState->window = SDL_CreateWindow("SSSpace", 1200, 800, SDL_WINDOW_VULKAN);
    return 0;
}

// Returns 0 if initialization successful
// Writes a pointer to created EngineState if successful
int Init_MainEngine(EngineState** esPointer)
{
    EngineState* engineState = (EngineState*)calloc(1, sizeof(EngineState));
    if (!SDL_Init(SDL_INIT_VIDEO)) goto SDL_Fail;

    if (Create_MainWindow(engineState)) goto SDL_Fail;
    printf("Main window created\n");
    if (Create_VulkanInstance(engineState)) {printf("Vulkan Instance failed\n"); goto SDL_Fail;}
    printf("Vulkan instance created\n");
    if (Get_PhysicalDevice(engineState)) {printf("Physical Device failed\n"); goto PhysicalDevice_Fail;}
    printf("Physical device aquired\n");
    if (Create_MainSurface(engineState)) {printf("Main Surface failed\n"); goto PhysicalDevice_Fail;}
    printf("Main surface created\n");
    if (Select_QueueFamilies(engineState)) {printf("QueueFamilies failed\n"); goto QueueFamilies_Fail;}
    printf("Queue families selected\n");
    if (Create_LogicalDevice(engineState)) {printf("Logical Device failed\n"); goto QueueFamilies_Fail;}
    printf("Logical device created\n");
    Get_QueueHandles(engineState);
    printf("Queue handles aquired\n");

    *esPointer = engineState;
    return 0;

    QueueFamilies_Fail: vkDestroySurfaceKHR(engineState->instance, engineState->surface, NULL);
    PhysicalDevice_Fail: vkDestroyInstance(engineState->instance, NULL);
    SDL_Fail: free(engineState);

    return -1;
}

int Cleanup_MainEngine(EngineState* engineState)
{
    Cleanup_Swapchain(engineState);
    free(engineState->queueHandles._Compute);
    free(engineState->queueHandles._Graphics);
    free(engineState->queueHandles._Sparse);
    free(engineState->queueHandles._Transfer);
    free(engineState->queueHandles._Present);
    vkDestroyDevice(engineState->device, NULL);
    vkDestroySurfaceKHR(engineState->instance, engineState->surface, NULL);
    vkDestroyInstance(engineState->instance, NULL);
    SDL_DestroyWindow(engineState->window);
    SDL_Quit();
    free(engineState->swapchainState);
    free(engineState);
    return 0;
}