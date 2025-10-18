#ifndef RENDER_PASSES
#include "engine_init.h"
#define RENDER_PASSES

int Submit_CommandBuffer(EngineState* engineState, int frame_ind, VkCommandBuffer Cmnd);

VkRenderingInfo Get_MainRenderPassInfo(Uint64 frameCount, VkRenderingAttachmentInfo* colorAttachments, EngineState* engineState);

VkRenderingAttachmentInfo Get_RenderAttachmentInfo(VkImageView imageView);

#endif