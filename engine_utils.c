#include "engine_utils.h"

ImmediateCommand Command_Immediate_Begin(VkDevice device, VkCommandBuffer cmnd, VkQueue queue)
{
    if (vkResetCommandBuffer(cmnd, 0) != VK_SUCCESS) {printf("!Failed to reset immediate command buffer\n"); return (ImmediateCommand){};}
    VkFence fence;
    VkFenceCreateInfo cInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
    };
    if (vkCreateFence(device, &cInfo, NULL, &fence) != VK_SUCCESS)
    {
        printf("!Failed to create fence for immediate submit\n");
    }

    VkCommandBufferBeginInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = NULL;
    bInfo.pInheritanceInfo = NULL;
    bInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;   

    if (vkBeginCommandBuffer(cmnd, &bInfo) != VK_SUCCESS) {printf("!Failed to begin immediate command buffer\n"); return (ImmediateCommand){};}
    return (ImmediateCommand){.fence = fence, .buffer = cmnd, .queue = queue, .device = device};
}

void Command_Immediate_End(ImmediateCommand imcmnd)
{
    vkEndCommandBuffer(imcmnd.buffer);
    VkSubmitInfo sInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = & imcmnd.buffer
    };

    if (vkQueueSubmit(imcmnd.queue, 1, &sInfo, imcmnd.fence) != VK_SUCCESS) printf("!Failed to submit immediate command buffer\n");
}

void Command_Immediate_Finish(ImmediateCommand imcmnd)
{
    if (vkWaitForFences(imcmnd.device, 1, &imcmnd.fence, VK_TRUE, 1000000000) != VK_SUCCESS) printf("!Immediate command fence timed out\n");
    vkDestroyFence(imcmnd.device, imcmnd.fence, NULL);
}

void Command_Immediate_Complete(ImmediateCommand imcmnd)
{
    Command_Immediate_End(imcmnd);
    Command_Immediate_Finish(imcmnd);
}