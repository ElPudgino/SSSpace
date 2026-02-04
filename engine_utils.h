#ifndef ENGINE_UTILS
#define ENGINE_UTILS

#include "libs.h"

typedef struct _ImmediateCommand
{
    VkFence fence;
    VkCommandBuffer buffer;
    VkQueue queue;
    VkDevice device;
} ImmediateCommand;

ImmediateCommand Command_Immediate_Begin(VkDevice device, VkCommandBuffer cmnd, VkQueue queue);

void Command_Immediate_End(ImmediateCommand imcmnd);

void Command_Immediate_Finish(ImmediateCommand imcmnd);

void Command_Immediate_Complete(ImmediateCommand imcmnd);

#endif