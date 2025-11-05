#include <stdio.h>
#include <math.h>
#include "swapchain_init.h"



int Create_VulkanInstance(EngineState* engineState)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SSSpace";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

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

int Destroy_VulkanInstance(EngineState* engineState)
{
    printf("Destroying Instance\n");
    vkDestroyInstance(engineState->instance, NULL);
    return 1;
}

int Get_PhysicalDevice(EngineState* engineState)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(engineState->instance, &deviceCount, NULL);
    VkPhysicalDevice* physDevices = (VkPhysicalDevice*)calloc(deviceCount, sizeof(VkPhysicalDevice));

    int res = 0;
    if ((res = vkEnumeratePhysicalDevices(engineState->instance, &deviceCount, physDevices)) != VK_SUCCESS)
    {
        printf("!!Failed to enumerate devices, error code: %d\n", res);
        free(physDevices);
        return -1;
    }

    if (deviceCount < 1)
    {
        printf("!!Failed find devices\n");
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

int Destroy_MainSurface(EngineState* engineState)
{
    printf("Destroying Surface\n");
    vkDestroySurfaceKHR(engineState->instance, engineState->surface, NULL);
    return 1;
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

    uint32_t extensionCount = 3;
    char const** extensions = (char const**)calloc(extensionCount, sizeof(char const**));
    extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    extensions[1] = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
    extensions[2] = VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME;

    // Add required extensions (like swapchain)
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;


    int res = 0;
    if ((res = vkCreateDevice(engineState->physicalDevice, &createInfo, NULL, &engineState->device)) != VK_SUCCESS) printf("Failed to create logical device, error code: %d\n", res);

    free(extensions);
    free(queueCreateInfos);
    return res;
}

int Destroy_LogicalDevice(EngineState* engineState)
{
    printf("Destroying Logical device\n");
    vkDestroyDevice(engineState->device, NULL);
    return 1;
}

int Get_QueueHandles(EngineState* engineState)
{
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Compute, 0, &engineState->queueHandles._Compute);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Graphics, 0, &engineState->queueHandles._Graphics);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Sparse, 0, &engineState->queueHandles._Sparse);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Transfer, 0, &engineState->queueHandles._Transfer);
    vkGetDeviceQueue(engineState->device, engineState->queueFamIndices._Present, 0, &engineState->queueHandles._Present);
    return 0;
}

int Create_CommandsHandle(EngineState* engineState)
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = engineState->queueFamIndices._Graphics;

    if (vkCreateCommandPool(engineState->device, &createInfo, NULL, &engineState->commandsHandle.commandPool) != VK_SUCCESS)
    {
        return -printf("!!Failed to create command pool\n");
    }

    VkCommandBufferAllocateInfo cmdAllocInfo = {};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.pNext = NULL;
    cmdAllocInfo.commandPool = engineState->commandsHandle.commandPool;
    cmdAllocInfo.commandBufferCount = _BufferCount;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(engineState->device, &cmdAllocInfo, engineState->commandsHandle.commandBuffers) != VK_SUCCESS)
    {
        return -printf("!!Failed to allocate command buffers\n");
    }
    return 0;
}

int Destroy_CommandsHandles(EngineState* engineState)
{
    printf("Destroying Command pool\n");
    vkDestroyCommandPool(engineState->device, engineState->commandsHandle.commandPool, NULL);
    return 1;
}

int Create_MainWindow(EngineState* engineState)
{
    engineState->window = SDL_CreateWindow("SSSpace", 1200, 800, SDL_WINDOW_VULKAN);
    return 0;
}

int Destroy_MainWindow(EngineState* engineState)
{
    printf("Destroying Main window\n");
    SDL_DestroyWindow(engineState->window);
    return 1;
}

int Create_SyncStructures(EngineState* engineState)
{
    VkFenceCreateInfo fcreateInfo = {};
    fcreateInfo.pNext = NULL;
    fcreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fcreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semcreateInfo = {};
    semcreateInfo.pNext = NULL;
    semcreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (int i = 0; i < _BufferCount; i++)
    {
        if (vkCreateFence(engineState->device, &fcreateInfo, NULL, &engineState->frameData.fence[i]) != VK_SUCCESS)
        {
            return -printf("!!Failed to create fence\n");
        }
        if (vkCreateSemaphore(engineState->device, &semcreateInfo, NULL, &engineState->frameData.swapchainSemaphore[i]) != VK_SUCCESS)
        {
            return -printf("!!Failed to create swapchain semaphore\n");
        }
        if (vkCreateSemaphore(engineState->device, &semcreateInfo, NULL, &engineState->frameData.computeSemaphore[i]) != VK_SUCCESS)
        {
            return -printf("!!Failed to create compute semaphore\n");
        } 
    }

    
    return 0; 
}

int Destroy_SyncStructures(EngineState* engineState)
{
    printf("Destroying Sync structures\n");
    for (int i = 0; i < _BufferCount; i++)
    {
        vkDestroyFence(engineState->device, engineState->frameData.fence[i], NULL);
        vkDestroySemaphore(engineState->device, engineState->frameData.swapchainSemaphore[i], NULL);
        vkDestroySemaphore(engineState->device, engineState->frameData.computeSemaphore[i], NULL);
    }
    return 1;
}

int Create_Allocator(EngineState* engineState)
{
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = engineState->physicalDevice;
    allocatorInfo.device = engineState->device;
    allocatorInfo.instance = engineState->instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    if (vmaCreateAllocator(&allocatorInfo, &engineState->allocator) != VK_SUCCESS) return -printf("Failed to init VMA\n");
    return 0;
}

int Destroy_Allocator(EngineState* engineState)
{
    printf("Destroying VMA\n");
    vmaDestroyAllocator(engineState->allocator);
    return 1;
}

// Adds a destructor to a singly linked list
// Order is FILO
int Add_ToCleanupQueue(AllocInfo** allocInfo, int (*cleanupFunc)(EngineState*))
{
    AllocInfo* n = (AllocInfo*)calloc(1, sizeof(AllocInfo));
    (*allocInfo)->CleanupFunc = cleanupFunc;
    n->next = *allocInfo; 
    *allocInfo = n;
    return 1;
}

// Create image to which everything will be rendered before being copied to swapchain
int Create_MainDrawImage(EngineState* engineState)
{
    return Create_Image(engineState->device, engineState->allocator, &engineState->frameData.drawImage, (VkExtent3D){1080, 720, 1});
}

int Destroy_MainDrawImage(EngineState* engineState)
{
    vkDestroyImageView(engineState->device, engineState->frameData.drawImage.imageView, NULL);
    vmaDestroyImage(engineState->allocator, engineState->frameData.drawImage.image, engineState->frameData.drawImage.allocation);
    return 1;
}

// Recursively calls allocInfos destructors
int Destroy_FromQueue(AllocInfo* allocInfo, EngineState* engineState)
{
    if (allocInfo->CleanupFunc) allocInfo->CleanupFunc(engineState);
    if (allocInfo->next) Destroy_FromQueue(allocInfo->next, engineState);
    free(allocInfo);
    return 1;
}

int Quit_SDL(EngineState* engineState)
{
    SDL_Quit();
    return 1;
}

int Dealloc_Engine(EngineState* engineState)
{
    free(engineState);
    return 1;
}

// Returns 0 if initialization successful
// Writes a pointer to created EngineState if successful
int Init_MainEngine(EngineState** esPointer, AllocInfo** allocInfo)
{
    EngineState* engineState = (EngineState*)calloc(1, sizeof(EngineState));
    Add_ToCleanupQueue(allocInfo, Dealloc_Engine);

    if (!SDL_Init(SDL_INIT_VIDEO)) goto Fail;
    Add_ToCleanupQueue(allocInfo, Quit_SDL);

    if (Create_MainWindow(engineState)) goto Fail;
    Add_ToCleanupQueue(allocInfo, Destroy_MainWindow);
    printf("Main window created\n");

    if (Create_VulkanInstance(engineState)) {printf("!!Vulkan Instance failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_VulkanInstance);
    printf("Vulkan instance created\n");

    if (Get_PhysicalDevice(engineState)) {printf("!!Physical Device failed\n"); goto Fail;}
    printf("Physical device acquired\n");

    if (Create_MainSurface(engineState)) {printf("!!Main Surface failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_MainSurface);
    printf("Main surface created\n");

    if (Select_QueueFamilies(engineState)) {printf("!!QueueFamilies failed\n"); goto Fail;}
    printf("Queue families selected\n");

    if (Create_LogicalDevice(engineState)) {printf("!!Logical Device failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_LogicalDevice);
    printf("Logical device created\n");

    if (Get_QueueHandles(engineState)) {printf("!!Queue handles failed\n"); goto Fail;}
    printf("Queue handles acquired\n");

    if (Create_CommandsHandle(engineState)) {printf("!!Commands handle failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_CommandsHandles);
    printf("Command buffers created\n");

    if (Create_SyncStructures(engineState)) {printf("!!Sync structs failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_SyncStructures);
    printf("Sync structs created\n");

    if (Create_Allocator(engineState)) {printf("!!Allocator failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_Allocator);
    printf("Allocator created\n");

    if (Create_Swapchain(engineState)) {printf("!!Swapchain failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Cleanup_Swapchain);
    printf("Swapchain created\n");

    if (Create_MainDrawImage(engineState)) {printf("!!Main draw image creation failed\n"); goto Fail;}
    Add_ToCleanupQueue(allocInfo, Destroy_MainDrawImage);
    printf("Main draw image created\n");

    *esPointer = engineState;
    return 0;
Fail:
    printf("Fatal error while initializing engine; shutting down\n");
    Cleanup_MainEngine(engineState, *allocInfo);
    return -1;
}

int Cleanup_MainEngine(EngineState* engineState, AllocInfo* allocInfo)
{
    printf("Starting cleanup\n");
    vkDeviceWaitIdle(engineState->device);
    Destroy_FromQueue(allocInfo, engineState);
    return 0;
}
