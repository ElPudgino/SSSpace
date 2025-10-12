#include <stdio.h>
#include <math.h>
#include "engine_init.h"
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
    if (VK_SUCCESS != vkAcquireNextImageKHR(engineState->device, engineState->swapchainState->swapchain, 10e9, engineState->sync.swapchainSemaphore[frame_ind], NULL, &scImageIndex))
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


    int h = 0, w = 0;
    SDL_GetWindowSizeInPixels(engineState->window, &w, &h);

    VkRect2D rect = {.offset = {.x = 0, .y = 0},
    .extent = {.width = w, .height = h}};

    VkRenderingInfo rInfo = {};
    rInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rInfo.flags = 0;
    rInfo.layerCount = 1;
    rInfo.pNext = NULL;
    rInfo.viewMask = 0;
    rInfo.renderArea = rect;
    rInfo.colorAttachmentCount = 1;
    
    VkRenderingAttachmentInfo raInfo = {};
    raInfo.pNext= NULL;
    raInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    raInfo.imageView = engineState->swapchainState->imageViews[scImageIndex];
    raInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    raInfo.resolveMode = VK_RESOLVE_MODE_NONE;
    raInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    raInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkClearValue clv = {};
    VkClearColorValue ccv = {};
    ccv.float32[0] = 1.0f - (float)(frameCount%100000) * 0.00001f;
    ccv.float32[2] = (float)(frameCount%100000) * 0.00001f;
    ccv.float32[3] = 1.0f;
    clv.color = ccv;
    raInfo.clearValue = clv;

    rInfo.pColorAttachments = &raInfo;
    rInfo.pDepthAttachment = NULL;
    rInfo.pStencilAttachment = NULL;


    if (vkBeginCommandBuffer(Cmnd, &bInfo) != VK_SUCCESS) return -printf("Failed to begin command buffer\n");

    //start first render pass
    Change_ImageFormat(Cmnd, engineState->swapchainState->images[scImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(Cmnd, &rInfo);


    vkCmdEndRendering(Cmnd);

    Change_ImageFormat(Cmnd, engineState->swapchainState->images[scImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(Cmnd);

    VkSemaphoreSubmitInfo csemInfo = {};
	csemInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	csemInfo.pNext = NULL;
	csemInfo.semaphore = engineState->sync.computeSemaphore[frame_ind];
	csemInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	csemInfo.deviceIndex = 0;
	csemInfo.value = 1;

    VkSemaphoreSubmitInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	semInfo.pNext = NULL;
	semInfo.semaphore = engineState->sync.swapchainSemaphore[frame_ind];
	semInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
	semInfo.deviceIndex = 0;
	semInfo.value = 1;

    VkCommandBufferSubmitInfo cmdSubmitInfo = {};
    cmdSubmitInfo.pNext = NULL;
    cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmdSubmitInfo.deviceMask = 0;
    cmdSubmitInfo.commandBuffer = Cmnd;

    VkSubmitInfo2 submitInfo = {};
    submitInfo.pNext = NULL;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &cmdSubmitInfo;
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &csemInfo;
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &semInfo;

    vkQueueSubmit2(*engineState->queueHandles._Graphics, 1, &submitInfo, engineState->sync.fence[frame_ind]);

    VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.pSwapchains = &engineState->swapchainState->swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &engineState->sync.computeSemaphore[frame_ind];
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &scImageIndex;

	vkQueuePresentKHR(*engineState->queueHandles._Graphics, &presentInfo);

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
    printf("%ld\n",frameCount);
    printf("Closing\n");
    Cleanup_MainEngine(engineState);
}
