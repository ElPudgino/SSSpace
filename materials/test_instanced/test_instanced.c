#include "../materials.h"
#include "../../texture_table.h"
#include "../../samplers.h"

Material* Test_Instanced_Mat_Build(EngineState* engineState)
{
    printf("Starting mat building\n");
    VkShaderModule fragment = Read_SpirvFile("spvs/test_instanced.frag.spv", engineState->device);
    VkShaderModule vertex = Read_SpirvFile("spvs/test_instanced.vert.spv", engineState->device);
    printf("Read spirv\n");

    MaterialBuilder* builder = Start_MaterialBuilder(engineState->device);
    printf("Started builder\n");
    //MatBuilder_AddParameter(builder, sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT);
    //printf("Added parameter\n");
    MatBuilder_SetFragmentShader(builder, fragment);
    MatBuilder_SetVertexShader(builder, vertex);
    printf("Set shaders\n");
    MatBuilder_AddParameter(builder, sizeof(MeshParameter), VK_SHADER_STAGE_VERTEX_BIT);
    MatBuilder_AddImageSlot(builder, 0, VK_SHADER_STAGE_FRAGMENT_BIT);

    Material* mat = Finish_MaterialBuilder(builder);

    ImageData tex = {};
    tex = *TextureTable_Get_Texture("blast_furnace_top.png");
    Material_SetImageSlot(mat, 0, tex, Get_PixelSampler());
    printf("Finished builder\n");
    //float val = 0.5f;
    //Material_SetParameter(mat, 0, &val);
    //printf("Set parameter\n");

    vkDestroyShaderModule(engineState->device, vertex, NULL);
    vkDestroyShaderModule(engineState->device, fragment, NULL);
    
    return mat;
}