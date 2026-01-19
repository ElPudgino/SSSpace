#ifndef SHADER_LOADER

#include "libs.h"

#define SHADER_LOADER

VkShaderModule Create_ShaderModule(char* data, VkDevice device, uint32_t size);

VkShaderModule Read_SpirvFile(char* name, VkDevice device);

#endif