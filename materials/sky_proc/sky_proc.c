#include "../materials.h"
#include "../../texture_table.h"
#include "../../samplers.h"
#include "../../vfx_params.h"

Material* Sky_proc_Mat_Build(EngineState* engineState)
{
    printf("Starting mat building\n");
    VkShaderModule fragment = Read_SpirvFile("spvs/sky_proc.frag.spv", engineState->device);
    VkShaderModule vertex = Read_SpirvFile("spvs/sky_proc.vert.spv", engineState->device);
    printf("Read spirv\n");

    MaterialBuilder* builder = Start_MaterialBuilder(engineState->device);
    printf("Started builder\n");

    MatBuilder_SetFragmentShader(builder, fragment);
    MatBuilder_SetVertexShader(builder, vertex);
    printf("Set shaders\n");
    MatBuilder_AddImageSlot(builder, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddParameter(builder, sizeof(mat4), VK_SHADER_STAGE_VERTEX_BIT);
    MatBuilder_AddParameter(builder, sizeof(VkDeviceAddress), VK_SHADER_STAGE_VERTEX_BIT);
    MatBuilder_SetDepthTest(builder, VK_FALSE);
    MatBuilder_SetColorBlending(builder, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);

    Material* mat = Finish_MaterialBuilder(builder);
    printf("Finished builder\n");
    //float val = 0.5f;

    ImageData* tex = NULL;
    tex = TextureTable_Get_Texture("star.png");
    if (tex) Material_SetImageSlot(mat, 0, *tex, Get_LinearSampler());
    else printf("!!Texture not found for sky material\n");

    vkDestroyShaderModule(engineState->device, vertex, NULL);
    vkDestroyShaderModule(engineState->device, fragment, NULL);
    
    return mat;
}