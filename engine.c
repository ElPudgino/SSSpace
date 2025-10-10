#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>
#include "engine_init.h"

int Run_MainLoop(EngineState* engineState)
{
    if (vkWaitForFences(engineState->device, 1, &engineState->sync.fence, true, 10e9) != VK_SUCCESS)
    {
        return -printf("Waiting for render fence failed\n");
    }
    if (vkResetFences(engineState->device, 1, &engineState->sync.fence))
    {
        return -printf("Reseting render fence failed\n");
    }


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

        if (Run_MainLoop(engineState) != VK_SUCCESS) running = 0;
    }

    printf("Closing\n");
    Cleanup_MainEngine(engineState);
}
