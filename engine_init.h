#ifndef ENGINE_INIT

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
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
    SDL_Window* window;
    SwapchainState* swapchainState;
} EngineState;


int Select_QueueFamilies(EngineState* engineState);

int Init_MainEngine(EngineState** esPointer);

int Cleanup_MainEngine(EngineState* engineState);

#endif