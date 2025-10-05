#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>
#include "engine_init.h"

int Run_MainLoop(int* running)
{
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

        Run_MainLoop(&running);
    }

    printf("Closing\n");
    Cleanup_MainEngine(engineState);
}
