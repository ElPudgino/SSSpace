#include "shader_loader.h"

VkShaderModule Create_ShaderModule(char* data, VkDevice device, uint32_t size)
{
    assert(data);
    uint32_t* sdata = (uint32_t*)data;

    VkShaderModuleCreateInfo cInfo = {};
    cInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    cInfo.pCode = sdata;
    cInfo.codeSize = size;

    VkShaderModule module;
    if (vkCreateShaderModule(device, &cInfo, NULL, &module) != VK_SUCCESS) 
    {
        printf("!Failed to create shader module\n");
        return NULL;
    }
    return module;
}

VkShaderModule Read_SpirvFile(char const* name, VkDevice device)
{
    assert(name);
    assert(name);

    FILE* file;
    file = fopen(name,"r");
    if (!file)
    {
        printf("!Shader file not found\n");
    }
    uint32_t size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size == 0) {printf("!Shader file has 0 length: %s\n", name); return NULL;}
    char* data = (char*)calloc(size, sizeof(char));
    char c; 
    fread(data, 1, size, file);

    VkShaderModule module = Create_ShaderModule(data, device, size);
    fclose(file);
    return module;
}


