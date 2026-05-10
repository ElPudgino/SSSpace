#include "../materials.h"
#include "../../texture_table.h"
#include "../../samplers.h"
#include "../../vfx_params.h"

Material* Sky_Mat_Build(EngineState* engineState)
{
    printf("Starting mat building\n");
    VkShaderModule fragment = Read_SpirvFile("spvs/sky.frag.spv", engineState->device);
    VkShaderModule vertex = Read_SpirvFile("spvs/sky.vert.spv", engineState->device);
    printf("Read spirv\n");

    MaterialBuilder* builder = Start_MaterialBuilder(engineState->device);
    printf("Started builder\n");

    MatBuilder_SetFragmentShader(builder, fragment);
    MatBuilder_SetVertexShader(builder, vertex);
    printf("Set shaders\n");
    MatBuilder_AddImageSlot(builder, 0, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddImageSlot(builder, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddImageSlot(builder, 2, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddImageSlot(builder, 3, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddImageSlot(builder, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
    MatBuilder_AddImageSlot(builder, 5, VK_SHADER_STAGE_FRAGMENT_BIT);

    MatBuilder_AddParameter(builder, sizeof(VkDeviceAddress), VK_SHADER_STAGE_VERTEX_BIT);
    MatBuilder_SetDepthTest(builder, VK_FALSE);
    MatBuilder_SetColorBlending(builder, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);

    Material* mat = Finish_MaterialBuilder(builder);
    printf("Finished builder\n");
    //float val = 0.5f;

    ImageData* tex = NULL;

    tex = TextureTable_Get_Texture("bkg3_right1.png");
    if (tex) Material_SetImageSlot(mat, 0, *tex, Get_LinearSampler());
    else printf("!!Texture 1 not found for sky material\n");

    tex = TextureTable_Get_Texture("bkg3_left2.png");
    if (tex) Material_SetImageSlot(mat, 1, *tex, Get_LinearSampler());
    else printf("!!Texture 2 not found for sky material\n");

    tex = TextureTable_Get_Texture("bkg3_top3.png");
    if (tex) Material_SetImageSlot(mat, 2, *tex, Get_LinearSampler());
    else printf("!!Texture 3 not found for sky material\n");

    tex = TextureTable_Get_Texture("bkg3_bottom4.png");
    if (tex) Material_SetImageSlot(mat, 3, *tex, Get_LinearSampler());
    else printf("!!Texture 4 not found for sky material\n");

    tex = TextureTable_Get_Texture("bkg3_front5.png");
    if (tex) Material_SetImageSlot(mat, 4, *tex, Get_LinearSampler());
    else printf("!!Texture 5 not found for sky material\n");

    tex = TextureTable_Get_Texture("bkg3_back6.png");
    if (tex) Material_SetImageSlot(mat, 5, *tex, Get_LinearSampler());
    else printf("!!Texture 6 not found for sky material\n");

    vkDestroyShaderModule(engineState->device, vertex, NULL);
    vkDestroyShaderModule(engineState->device, fragment, NULL);
    
    return mat;
}