#include <stdio.h>
#include <math.h>
#include "render_passes.h"
#include "engine_utils.h"

int Run_MainLoop(EngineState* engineState, Uint64 frameCount)
{
    int frame_ind = frameCount % _BufferCount;


    if (vkWaitForFences(engineState->device, 1, &engineState->sync.fence[frame_ind], true, 10e9) != VK_SUCCESS)
    {
        return -printf("Waiting for render fence failed\n");
    }
    if (vkResetFences(engineState->device, 1, &engineState->sync.fence[frame_ind]))
    {
        return -printf("Reseting render fence failed\n");
    }
    uint32_t scImageIndex = 0;
    if (VK_SUCCESS != vkAcquireNextImageKHR(engineState->device, engineState->swapchainState.swapchain, 10e9, engineState->sync.swapchainSemaphore[frame_ind], NULL, &scImageIndex))
    {
        return -printf("Failed to acquire next swapchain image\n");;
    }

    // start command buffer
    VkCommandBuffer Cmnd = engineState->commandsHandle.commandBuffers[frame_ind];

    if (vkResetCommandBuffer(Cmnd, 0) != VK_SUCCESS) return -printf("Failed to reset command buffer\n");

    VkCommandBufferBeginInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = NULL;
    bInfo.pInheritanceInfo = NULL;
    bInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    
    if (vkBeginCommandBuffer(Cmnd, &bInfo) != VK_SUCCESS) return -printf("Failed to begin command buffer\n");

    //start first render pass
    Change_ImageFormat(Cmnd, engineState->swapchainState.images[scImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingAttachmentInfo raInfo = Get_RenderAttachmentInfo(engineState->swapchainState.imageViews[scImageIndex]);
    VkRenderingInfo rInfo = Get_MainRenderPassInfo(frameCount, &raInfo, engineState);
    vkCmdBeginRendering(Cmnd, &rInfo);


    vkCmdEndRendering(Cmnd);

    Change_ImageFormat(Cmnd, engineState->swapchainState.images[scImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(Cmnd);


    Submit_CommandBuffer(engineState, frame_ind, Cmnd);

    VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.pSwapchains = &engineState->swapchainState.swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &engineState->sync.computeSemaphore[frame_ind];
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &scImageIndex;

	vkQueuePresentKHR(engineState->queueHandles._Graphics, &presentInfo);

    return 0;
}

int main(int argc, char** argv)
{
    int running = 1;

    EngineState* engineState;
    AllocInfo* allocInfo = (AllocInfo*)calloc(1, sizeof(AllocInfo));
    int res = 0;
    if ((res = Init_MainEngine(&engineState, &allocInfo))) return -printf("Main engine initialization failed, exit code: %d\n", res);

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
    printf("%ld\n",frameCount);
    printf("Closing\n");
    Cleanup_MainEngine(engineState, allocInfo);
}
