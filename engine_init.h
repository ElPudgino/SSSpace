#ifndef ENGINE_INIT

#include "libs.h"

#define ENGINE_INIT

typedef struct _QueueHandles
{
    VkQueue* _Graphics;
    VkQueue* _Compute;
    VkQueue* _Sparse;
    VkQueue* _Transfer;
    VkQueue* _Present;
} QueueHandles;

typedef struct _SwapchainState
{
    VkSwapchainKHR swapchain;
    VkImage* images;
    VkImageView* imageViews;
    uint32_t imageCount;
} SwapchainState;

#define _BufferCount 2

typedef struct _CommandsHandle
{
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffers[_BufferCount];
} CommandsHandle;

typedef struct _Sync
{
    VkFence fence[_BufferCount];
    VkSemaphore swapchainSemaphore[_BufferCount];
    VkSemaphore computeSemaphore[_BufferCount];
} Sync;

typedef struct _QueueFamIndices
{
    uint32_t _Graphics;
    uint32_t _Compute;
    uint32_t _Sparse;
    uint32_t _Transfer;
    uint32_t _Present;
} QueueFamIndices;

typedef struct _EngineState
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    QueueFamIndices queueFamIndices;
    QueueHandles queueHandles;
    CommandsHandle commandsHandle;
    Sync sync;
    SDL_Window* window;
    SwapchainState* swapchainState;
    VmaAllocator allocator;
} EngineState;


int Select_QueueFamilies(EngineState* engineState);

int Init_MainEngine(EngineState** esPointer);

int Cleanup_MainEngine(EngineState* engineState);

#endif
