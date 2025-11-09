#ifndef ENGINE_INIT

#include "image_utils.h"
#include "descriptors_util.h"

#define ENGINE_INIT

typedef struct _DescriptorCache
{
    // Data like the draw image, camera transform and other things which dont change throughout the frame
    DescriptorData frameShaderData;
} DescriptorCache;

typedef struct _QueueHandles
{
    VkQueue _Graphics;
    VkQueue _Compute;
    VkQueue _Sparse;
    VkQueue _Transfer;
    VkQueue _Present;
} QueueHandles;

typedef struct _SwapchainState
{
    VkSwapchainKHR swapchain;
    VkExtent3D extent;
    VkImage* images;
    VkImageView* imageViews;
    uint32_t imageCount;
    int created;
} SwapchainState;

#define _BufferCount 2

typedef struct _CommandsHandle
{
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffers[_BufferCount];
} CommandsHandle;

typedef struct _FrameData
{
    VkFence fence[_BufferCount];
    VkSemaphore swapchainSemaphore[_BufferCount];
    VkSemaphore computeSemaphore[_BufferCount];
    ImageData drawImage;
} FrameData;

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
    FrameData frameData;
    SDL_Window* window;
    SwapchainState swapchainState;
    VmaAllocator allocator;
} EngineState;

typedef struct _AllocInfo
{
    int (*CleanupFunc)(EngineState*);
    struct _AllocInfo* next;
} AllocInfo;

int Select_QueueFamilies(EngineState* engineState);

int Init_MainEngine(EngineState** esPointer, AllocInfo** allocInfo);

int Cleanup_MainEngine(EngineState* engineState, AllocInfo* allocInfo);

#endif
