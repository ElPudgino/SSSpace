#ifndef RENDER_PRIMITIVES
#define RENDER_PRIMITIVES

#include "libs.h"
#include "pipeline_builder.h"
#include "image_utils.h"
#include "descriptors_util.h"

/*! @brief Material data which is updated every frame via push constants
* @param size Size in bytes of the parameter
* @param offset Offset in bytes of the parameter. Handled by material builder
* @param stage Shader stage which uses this parameter
* @param value Pointer to the parameter value. Allocation is handled by material builder
*/
typedef struct _MaterialParameter
{
    uint32_t size;
    uint32_t offset;
    VkShaderStageFlags stage;
    void* value;
} MaterialParameter;

/*! @brief Abstraction to simplify handling shaders
* Fields shouldn't be modified directly. Creation is handled by Material builder
* Last descriptor set in descSets is this materials unique set, other ones are shared
* Order of material parameters is the same as the order they were added in the builder
* @param device Logic device on which material was created
* @param pipeline Graphics pipeline which includes shaders
* @param ownPool Descriptor pool from which the materials unique descriptor set is allocated
* @param ownLayout Descriptor layout of the materials unique descriptor set
* @param pLayout Pipeline layout of the materials graphics pipeline
* @param parameters Array of this materials parameters
* @param parameterCount Length of the parameters array
*/
typedef struct _Material
{
    VkDevice device;
    VkPipeline pipeline;
    VkDescriptorPool ownPool;
    VkDescriptorSetLayout ownLayout;
    VkDescriptorSet descSets[4]; // Last set is material own unique set 
    VkPipelineLayout pLayout;
    MaterialParameter* parameters;
    uint32_t parameterCount;
} Material;


/*! @brief Stores all intermediate data used in material creation
* Fields shouldn't be modified directly.
* matParams array will be directly given to the created material, it is freed with the material and not the builder.
* @param device Logic device on which material was created
* @param pipelineBuilder Handles creation of the graphics pipeline for material
* @param pipelineLayoutBuilder Handles creation of pipelines layout
* @param descLayoutBuilder Handles creation of the materials unique descriptor set layout
* @param poolSizes Stores individual descriptor amounts in the unique descriptor set
* @param curParamOffset Internal variable used to track which offset each parameter should have
* @param matParams Array which stores created materials parameters
* @param matParamsCount Length of the parameters array
*/
typedef struct _MaterialBuilder
{
    VkDevice device;
    PipelineBuilder* pipelineBuilder;
    PipelineLayoutBuilder* pipelineLayoutBuilder;
    DescriptorLayoutBuilder* descLayoutBuilder;
    DescriptorPoolSizes poolSizes;
    uint32_t curParamOffset;
    MaterialParameter* matParams;
    uint32_t matParamsCount;
} MaterialBuilder;

MaterialBuilder* Start_MaterialBuilder(VkDevice device);

void MatBuilder_SetFragmentShader(MaterialBuilder* builder, VkShaderModule shader);

void MatBuilder_SetVertexShader(MaterialBuilder* builder, VkShaderModule shader);

void MatBuilder_AddImageSlot(MaterialBuilder* builder, uint32_t bind, VkShaderStageFlags stage);

void MatBuilder_AddParameter(MaterialBuilder* builder, uint32_t paramSize, VkShaderStageFlags stage);

Material Finish_MaterialBuilder(MaterialBuilder* builder);

void Material_SetParameter(Material mat, uint32_t index, void* value);

void Material_SetImageSlot(Material mat, uint32_t bind, ImageData imageData);

void Bind_Material(VkCommandBuffer cmnd, Material material);

#endif