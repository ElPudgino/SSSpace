#include "engine_init.h"

int Submit_CommandBuffer(EngineState* engineState, int frame_ind, VkCommandBuffer Cmnd)
{
    assert(engineState);
    VkSemaphoreSubmitInfo csemInfo = {};
	csemInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	csemInfo.pNext = NULL;
	csemInfo.semaphore = engineState->frameData.computeSemaphore[frame_ind];
	csemInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	csemInfo.deviceIndex = 0;
	csemInfo.value = 1;

    VkSemaphoreSubmitInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	semInfo.pNext = NULL;
	semInfo.semaphore = engineState->frameData.swapchainSemaphore[frame_ind];
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

    vkQueueSubmit2(engineState->queueHandles._Graphics, 1, &submitInfo, engineState->frameData.fence[frame_ind]);
    return 0;
}

VkRenderingInfo Get_MainRenderPassInfo(Uint64 frameCount, VkRenderingAttachmentInfo* colorAttachments, EngineState* engineState)
{
    assert(engineState);
    assert(colorAttachments);
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

    rInfo.pColorAttachments = colorAttachments;
    rInfo.pDepthAttachment = NULL;
    rInfo.pStencilAttachment = NULL;
    return rInfo;
}

VkRenderingAttachmentInfo Get_RenderAttachmentInfo(VkImageView imageView)
{
    VkRenderingAttachmentInfo raInfo = {};
    raInfo.pNext= NULL;
    raInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    raInfo.imageView = imageView; 
    raInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    raInfo.resolveMode = VK_RESOLVE_MODE_NONE;
    raInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    raInfo.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    return raInfo;
}