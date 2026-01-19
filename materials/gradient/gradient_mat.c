#include "render_primitives.h"
#include "shader_loader.h"
#include "engine_init.h"

Material* Gradient_Mat_Build(EngineState* engineState)
{
    VkShaderModule fragment = Read_SpirvFile("gradient_frag.spv", engineState->device);
    VkShaderModule vertex = Read_SpirvFile("gradient_vert.spv", engineState->device);

    MaterialBuilder* builder = Start_MaterialBuilder(engineState->device);
    MatBuilder_AddImageSlot(builder, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddParameter(builder, sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_SetFragmentShader(builder, fragment);
    MatBuilder_SetVertexShader(builder, vertex);

    Material* mat = Finish_MaterialBuilder(builder);
    float val = 0.5f;
    Material_SetParameter(mat, 0, &val);
    return mat;
}