#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>
#include "engine_init.h"

int Run_MainLoop(EngineState* engineState, Uint64 frameCount)
{
    int frame_ind = frameCount % 2;

    if (vkWaitForFences(engineState->device, 1, &engineState->sync.fence[frame_ind], true, 10e9) != VK_SUCCESS)
    {
        return -printf("Waiting for render fence failed\n");
    }
    if (vkResetFences(engineState->device, 1, &engineState->sync.fence[frame_ind]))
    {
        return -printf("Reseting render fence failed\n");
    }

    // start command buffer
    VkCommandBuffer Cmnd = engineState->commandsHandle.commandBuffers[frame_ind];

    vkResetCommandBuffer(Cmnd, 0);

    VkCommandBufferBeginInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = NULL;
    bInfo.pInheritanceInfo = NULL;
    bInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(Cmnd, &bInfo);

    //start first render pass



    return 0;
}

int main(int argc, char** argv)
{
    int running = 1;

    EngineState* engineState;
    int res = 0;
    if ((res = Init_MainEngine(&engineState))) return -printf("Main engine initialization failed, exit code: %d\n", res);

    printf("Starting main loop\n");
    SDL_Event event;

    Uint64 frameCount = 0;
    while (running)
    {
        while (SDL_PollEvent(&event)) // process all input before main update
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                printf("Quit event\n");
                running = 0;
            }
        }

        if (Run_MainLoop(engineState, frameCount) != VK_SUCCESS) running = 0;
        frameCount++;
    }

    printf("Closing\n");
    Cleanup_MainEngine(engineState);
}
