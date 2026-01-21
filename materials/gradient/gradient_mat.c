#include "../materials.h"

Material* Gradient_Mat_Build(EngineState* engineState)
{
    printf("Starting mat building\n");
    VkShaderModule fragment = Read_SpirvFile("spvs/gradient_frag.spv", engineState->device);
    VkShaderModule vertex = Read_SpirvFile("spvs/gradient_vert.spv", engineState->device);
    printf("Read spirv\n");

    MaterialBuilder* builder = Start_MaterialBuilder(engineState->device);
    printf("Started builder\n");
    //MatBuilder_AddParameter(builder, sizeof(float), VK_SHADER_STAGE_FRAGMENT_BIT);
    //printf("Added parameter\n");
    MatBuilder_SetFragmentShader(builder, fragment);
    MatBuilder_SetVertexShader(builder, vertex);
    printf("Set shaders\n");

    Material* mat = Finish_MaterialBuilder(builder);
    printf("Finished builder\n");
    //float val = 0.5f;
    //Material_SetParameter(mat, 0, &val);
    //printf("Set parameter\n");

    vkDestroyShaderModule(engineState->device, vertex, NULL);
    vkDestroyShaderModule(engineState->device, fragment, NULL);
    
    return mat;
}