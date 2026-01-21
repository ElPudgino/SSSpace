#include <stdio.h>
#include <math.h>
#include "render_passes.h"
#include "image_utils.h"
#include "materials/materials.h"

// Testing
Material* gradient = NULL;

void setup_mats(EngineState* engineState)
{
    gradient = Gradient_Mat_Build(engineState);
}

void _Testing(EngineState* engineState)
{
    setup_mats(engineState);
}

int Run_MainLoop(EngineState* engineState, Uint64 frameCount)
{
    int frame_ind = frameCount % _BufferCount;

    int error = 0;
    if ((error = vkWaitForFences(engineState->device, 1, &engineState->frameData.fence[frame_ind], true, 10e9)) != VK_SUCCESS)
    {
        return -printf("Waiting for render fence failed: %d\n",error);
    }
    if (vkResetFences(engineState->device, 1, &engineState->frameData.fence[frame_ind]) != VK_SUCCESS)
    {
        return -printf("Reseting render fence failed\n");
    }
    uint32_t scImageIndex = 0;
    if (VK_SUCCESS != vkAcquireNextImageKHR(engineState->device, engineState->swapchainState.swapchain, 10e9, engineState->frameData.swapchainSemaphore[frame_ind], NULL, &scImageIndex))
    {
        return -printf("Failed to acquire next swapchain image\n");
    }

    // start command buffer
    VkCommandBuffer Cmnd = engineState->commandsHandle.commandBuffers[frame_ind];
    ImageData DrawImage = engineState->frameData.drawImage;
    ImageData ScImage = (ImageData){
        .image = engineState->swapchainState.images[scImageIndex],
        .imageView = engineState->swapchainState.imageViews[scImageIndex],
        .layout = VK_IMAGE_LAYOUT_UNDEFINED,
        .imageExtent = engineState->swapchainState.extent};

    if (vkResetCommandBuffer(Cmnd, 0) != VK_SUCCESS) return -printf("Failed to reset command buffer\n");

    VkCommandBufferBeginInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = NULL;
    bInfo.pInheritanceInfo = NULL;
    bInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;   

    VkRenderingAttachmentInfo raInfo = Get_RenderAttachmentInfo(DrawImage.imageView);
    VkRenderingInfo rInfo = Get_MainRenderPassInfo(frameCount, &raInfo, engineState);

    //Start first render pass
    if (vkBeginCommandBuffer(Cmnd, &bInfo) != VK_SUCCESS) return -printf("Failed to begin command buffer\n");

    //Clear image
    Change_ImageLayout(Cmnd, &DrawImage, VK_IMAGE_LAYOUT_GENERAL);
    Clear_Image(Cmnd, DrawImage, (VkClearColorValue){0.4f,0.0f,0.0f,1.0f});
    Change_ImageLayout(Cmnd, &DrawImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    //Main rendering
    vkCmdBeginRendering(Cmnd, &rInfo);


    Bind_Material(Cmnd, gradient);

    VkExtent3D _drawExtent = engineState->frameData.drawImage.imageExtent;

    VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = _drawExtent.width;
	viewport.height = _drawExtent.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(Cmnd, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = _drawExtent.width;
	scissor.extent.height = _drawExtent.height;

	vkCmdSetScissor(Cmnd, 0, 1, &scissor);

    vkCmdDraw(Cmnd, 3, 1, 0, 0);

    vkCmdEndRendering(Cmnd);

    //Copy draw image to swapchain
    Change_ImageLayout(Cmnd, &DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    Change_ImageLayout(Cmnd, &ScImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Copy_ImageToImage(Cmnd, DrawImage, ScImage);
    
    //Prepare image for presentation
    Change_ImageLayout(Cmnd, &ScImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(Cmnd);


    Submit_CommandBuffer(engineState, frame_ind, Cmnd);

    VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.pSwapchains = &engineState->swapchainState.swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &engineState->frameData.computeSemaphore[frame_ind];
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &scImageIndex;

    vkQueueWaitIdle(engineState->queueHandles._Present); // Questionable solution for the following problem:
    /*
    1. first time command buffer is submitted (sets a fence)
    2. presentation starts (no fence is set)
    -- next frame (actually frame after the next because everything alternates between 2 frames)
    3. fence is waited for
    4(error). second time command buffer is submitted, BUT presentation from previous frame may not have finished
    */
	vkQueuePresentKHR(engineState->queueHandles._Present, &presentInfo);

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

    _Testing(engineState);

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

    Destroy_Material(gradient);

    printf("%ld\n",frameCount);
    printf("Closing\n");
    Cleanup_MainEngine(engineState, allocInfo);
}
