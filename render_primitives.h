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

/*! @brief Create a material builder instance
* Builder resources are freed when Finish is called
* @param device Logic device on which materials will be created
*/
MaterialBuilder* Start_MaterialBuilder(VkDevice device);

/*! @brief Set fragment shader of the materials pipeline
* Shader module should not be destroyed until builder is finished
* @param builder Target builder
* @param shader Used shader module
*/
void MatBuilder_SetFragmentShader(MaterialBuilder* builder, VkShaderModule shader);

/*! @brief Set vertex shader of the materials pipeline
* Shader module should not be destroyed until builder is finished
* @param builder Target builder
* @param shader Used shader module
*/
void MatBuilder_SetVertexShader(MaterialBuilder* builder, VkShaderModule shader);

/*! @brief Add a slot for an image to the pipeline
* Image itself will need to be bound after material has been created 
* @param builder Target builder
* @param bind Binding index, must be unique
* @param stage Shader stage
*/
void MatBuilder_AddImageSlot(MaterialBuilder* builder, uint32_t bind, VkShaderStageFlags stage);

/*! @brief Add a parameter to the pipeline
* Parameters are not reset automatically and need not be set each frame
* Parameters are will be sent as Push Constants
* Total size of parameters in bytes should not exceed 128 bytes
* @param builder Target builder
* @param paramSize Size of the parameter in bytes
* @param stage Shader stage
*/
void MatBuilder_AddParameter(MaterialBuilder* builder, uint32_t paramSize, VkShaderStageFlags stage);

/*! @brief Finish building a material
* Builder can not be used after finishing
* @param builder Target builder
*/
Material Finish_MaterialBuilder(MaterialBuilder* builder);

/*! @brief Set value of a material parameter
* @param mat Target material
* @param index Index of the parameter
* @param value Pointer from which value will be copied
*/
void Material_SetParameter(Material mat, uint32_t index, const void* value);

/*! @brief Link an image to a slot in the pipeline
* @param mat Target material
* @param bind Binding index of the slot
* @param imageData Image to be bound
*/
void Material_SetImageSlot(Material mat, uint32_t bind, ImageData imageData);

/*! @brief Bind material for rendering
* Binds materials pipeline and descriptors and pushes parameters
* @param cmnd Target command buffer
* @param material Material to be bound
*/
void Bind_Material(VkCommandBuffer cmnd, Material material);

#endif