#include "transform_utils.h"
#include "mesh_utils.h"

uint32_t CurrentArrayIndex = 1;
TransformArray* GlobalTransformArrays = NULL;
uint32_t GlobalTransformCap = 0;
VkDeviceAddress g_Transforms = 0;

BufferInfo TransformBuffer = {};
mat4* FullTransformArray = NULL;

void _add_RenderTransform(TransformArray* array, mat4 matrix);

void Render_InstancedMeshes(EngineState* engineState, VkCommandBuffer cmnd)
{
    // iterate all things that need to be rendered, then upload transforms

    Upload_Transforms(engineState);
    for (int i = 0; i < CurrentArrayIndex - 1; i++)
    {
        InstancedRenderData data = GlobalTransformArrays[i].renderData;
        //printf("inst data: %d | %d\n", GlobalTransformArrays[i].count, data.transformStartIndex);
        MeshParameter p =  {.meshAddress = data.mesh->g_Address, .transformAddress = g_Transforms, .transformStartIndex = data.transformStartIndex};
        Material_SetParameter(data.material, 0, &p);
        Bind_Material(cmnd, data.material);
        vkCmdBindIndexBuffer(cmnd, data.mesh->g_Indices.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmnd, data.mesh->indexCount, GlobalTransformArrays[i].count, 0, 0, 0);
        GlobalTransformArrays[i].count = 0;
    }
}

int Setup_TransformBuffer(EngineState* engineState, uint32_t size)
{
    TransformBuffer = CreateBuffer(engineState->allocator, size, 
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        (VmaAllocationCreateFlagBits)0,
        (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT));
    if (!TransformBuffer.buffer) {printf("!Failed to setup transform buffer (gpu alloc fail)\n"); return 1;}

    int res = 0;
    if((res = vmaMapMemory(engineState->allocator, TransformBuffer.allocation, (void**)&FullTransformArray)) != VK_SUCCESS) 
    {
        return -printf("!Failed to setup transform buffer (map fail: %d)\n", res);
    }
    VkBufferDeviceAddressInfo info = {.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = TransformBuffer.buffer};
    g_Transforms = vkGetBufferDeviceAddress(engineState->device, &info);
    return 0;
}

int Destroy_TransformBuffer(EngineState* engineState)
{
    vmaUnmapMemory(engineState->allocator, TransformBuffer.allocation);
    vmaDestroyBuffer(engineState->allocator, TransformBuffer.buffer, TransformBuffer.allocation);
    return 1;
}

void Upload_Transforms(EngineState* engineState)
{
    uint32_t curindex = 0;
    for (int i = 0;i < CurrentArrayIndex-1;i++)
    {
        // add checks for buffer size
        memcpy(FullTransformArray + curindex * sizeof(mat4), GlobalTransformArrays[i].array, sizeof(mat4) * GlobalTransformArrays[i].count);
        GlobalTransformArrays[i].renderData.transformStartIndex = curindex;
        curindex += GlobalTransformArrays[i].count;
    }
    int res = vmaFlushAllocation(engineState->allocator, TransformBuffer.allocation, 0, curindex * sizeof(mat4));
    if (res != VK_SUCCESS) printf("!Failed to flush allocation: %d\n", res);
}

void Add_TransformArray(InstancedRenderData* data)
{
    assert(data);
    assert(data->ID == 0);
    data->ID = CurrentArrayIndex;

    if (!GlobalTransformArrays)
    {
        GlobalTransformArrays = (TransformArray*)calloc(4, sizeof(TransformArray));
        GlobalTransformCap = 4;
    }
    else if (GlobalTransformCap == CurrentArrayIndex - 1)
    {
        GlobalTransformArrays = (TransformArray*)realloc(GlobalTransformArrays, sizeof(TransformArray) * GlobalTransformCap * 2);
        GlobalTransformCap *= 2;
    }
    GlobalTransformArrays[CurrentArrayIndex-1].renderData = *data;
    CurrentArrayIndex++;
}

void Add_InstanceToRender(InstancedRenderData* data, mat4 trs)
{
    assert(data);
    assert(data->ID > 0);
    _add_RenderTransform(&GlobalTransformArrays[data->ID-1], trs);
}

void _add_RenderTransform(TransformArray* array, mat4 matrix)
{
    assert(array);
    if (array->cap == 0)
    {
        array->cap = 4;
        array->array = (mat4*)calloc(4, sizeof(mat4));
    }
    else if (array->cap == array->count)
    {
        uint32_t newsize = (uint32_t)((float)array->cap * TRM_ARRAY_RESIZE_COEF);
        array->array = (mat4*)realloc(array->array, sizeof(mat4) * newsize);
        array->cap = newsize;
    }
    glm_mat4_copy(matrix, array->array[array->count]);
    array->count++;
}