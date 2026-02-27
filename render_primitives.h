#ifndef RENDER_PRIMITIVES
#define RENDER_PRIMITIVES

#include "libs.h"
#include "pipeline_builder.h"
#include "image_utils.h"
#include "descriptors_util.h"
#include "engine_init.h"
#include "vmem_util.h"

#define MATERIAL_DESC_SET_COUNT 4

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
    VkDescriptorSet descSets[MATERIAL_DESC_SET_COUNT]; // Last set is material own unique set 
    VkPipelineLayout pLayout;
    MaterialParameter* parameters;
    uint32_t parameterCount;
    uint32_t ID;
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

// !!!! Be careful with alignment
// vec3 is aligned to 16 bytes like vec4
// 2 vec2 can be packed into 1 16 bytes slot
typedef struct _Vertex
{
    vec3 position;
    float extra;
    vec2 normal;
    vec2 uv;
} Vertex;

/*! @brief Stores a unique mesh
* @attention When mesh is destroyed vertices and indices are freed
* @param engineState Reference to the main engine
* @param g_Vertices Vertex buffer, created when mesh is uploaded
* @param g_Indices Index buffer, created when mesh is uploaded
* @param g_Address GPU pointer, created when mesh is uploaded
* @param indices CPU side Index array, set manually
* @param indexCount Amount of elements in indices array, set manually
* @param indexCap Capacity of indices array, set manually
* @param vertices CPU side Vertex array, set manually
* @param vertexCount Amount of elements in vertices array, set manually
* @param vertexCap Capacity of vertices array, set manually
*/
typedef struct _Mesh
{
    EngineState* engineState;
    struct _BufferInfo g_Vertices;
    struct _BufferInfo g_Indices;
    VkDeviceAddress g_Address;
    uint32_t* indices;
    uint32_t indexCount;
    uint32_t indexCap;
    Vertex* vertices;
    uint32_t vertexCount;
    uint32_t vertexCap;
    int deleteWithRenderInstance;
} Mesh;

/*! @brief Contains data needed to render instances
* One InstancedRenderData is used per combination of Mesh+Material
* @param mesh Used mesh
* @param material Used material
* @param ID Index used to get corresponding TransformArray
* @param transformStartIndex Index of the first transform used in the shader
*/
typedef struct _InstancedRenderData
{
    Mesh* mesh;
    Material* material; 
    uint32_t ID;
    uint32_t transformStartIndex;
    int updateNeeded;
} InstancedRenderData;

/*! @brief Data passed to the shader when rendering
* @param meshAddress GPU pointer to the mesh vertex array
* @param transformAddress GPU pointer to the transform array
* @param transformStartIndex Index of the transform which will be used by the first instance
*/
typedef struct _MeshParameter
{
    VkDeviceAddress meshAddress;
    VkDeviceAddress transformAddress;
    uint32_t transformStartIndex;
} MeshParameter;

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
* Material should be destroyed later with Destroy_Material()
* @param builder Target builder
*/
Material* Finish_MaterialBuilder(MaterialBuilder* builder);

/*! @brief Set value of a material parameter
* @param mat Target material
* @param index Index of the parameter
* @param value Pointer from which value will be copied
*/
void Material_SetParameter(Material* mat, uint32_t index, const void* value);

/*! @brief Link an image to a slot in the pipeline
* @param mat Target material
* @param bind Binding index of the slot
* @param imageData Image to be bound
*/
void Material_SetImageSlot(Material* mat, uint32_t bind, ImageData imageData);

/*! @brief Bind material for rendering
* Binds materials pipeline and descriptors and pushes parameters
* @param cmnd Target command buffer
* @param material Material to be bound
*/
void Bind_Material(VkCommandBuffer cmnd, Material* material);

/*! @brief Destroy material and free its resources
* @param material Material to be destroyed
*/
void Destroy_Material(Material* mat);

#endif