#ifndef SHADER_LOADER

#include "libs.h"

#define SHADER_LOADER

VkShaderModule Create_ShaderModule(char* data, VkDevice device, uint32_t size);

VkShaderModule Read_SpirvFile(char const* name, VkDevice device);

#endif