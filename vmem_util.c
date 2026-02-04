#include "engine_utils.h"
#include "engine_init.h"
#include "render_primitives.h"

typedef struct _BufferInfo
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
} BufferInfo;

typedef struct _Vertex
{
    vec3 position;
    vec2 uv;
    vec2 normal;
} Vertex;

typedef struct _Mesh
{
    EngineState* engineState;
    struct _BufferInfo g_Vertices;
    struct _BufferInfo g_Indices;
    VkDeviceAddress g_Address;
    uint32_t* indices;
    size_t indexCount;
    Vertex* vertices;
    size_t vertexCount;
} Mesh;

typedef struct _MeshParameter
{
    mat4 worldMatrix;
    VkDeviceAddress meshAddress;
} MeshParameter;

void Mesh_UploadData(Mesh* mesh)
{
    size_t ind = mesh->indexCount * sizeof(size_t);
    size_t ver = mesh->vertexCount * sizeof(Vertex);
    size_t tem = ind + ver;

    BufferInfo temp = CreateBuffer(mesh->engineState->allocator, tem, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    BufferInfo vertexbuf = CreateBuffer(mesh->engineState->allocator, ver, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    BufferInfo indexbuf = CreateBuffer(mesh->engineState->allocator, ind, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // Get vertex array gpu pointer
    // We dont need index array pointer though
    VkBufferDeviceAddressInfo info = {.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = vertexbuf.buffer};
    mesh->g_Address = vkGetBufferDeviceAddress(mesh->engineState->device, &info);

    // Map gpu memory with temp buffer to cpu array 
    void* mem = NULL;
    VkDeviceMemory dmem = temp.allocInfo.deviceMemory;
    vkMapMemory(mesh->engineState->device, dmem, 0, tem, 0, &mem);
    

    memcpy(mem, mesh->vertices, ver);
    memcpy(mem + mesh->vertexCount * sizeof(Vertex), mesh->indices, ind);

    ImmediateCommand IC = Command_Immediate_Begin(mesh->engineState->device, mesh->engineState->commandsHandle.miscUpdateBuffer, mesh->engineState->queueHandles._Graphics);
    
    VkBufferCopy vertexCopy = {};
    vertexCopy.dstOffset = 0;
    vertexCopy.srcOffset = 0;
    vertexCopy.size = mesh->vertexCount * sizeof(Vertex);

    vkCmdCopyBuffer(IC.buffer, temp.buffer, vertexbuf.buffer, 1, &vertexCopy);

    VkBufferCopy indexCopy = {};
    indexCopy.dstOffset = 0;
    indexCopy.srcOffset = mesh->vertexCount * sizeof(Vertex);
    indexCopy.size = ind;

    vkCmdCopyBuffer(IC.buffer, temp.buffer, indexbuf.buffer, 1, &indexCopy);

    Command_Immediate_Complete(IC);
    vkDestroyBuffer(mesh->engineState->device, temp.buffer, NULL);

    if (mesh->g_Vertices.buffer != NULL)
    {
        vkDestroyBuffer(mesh->engineState->device, mesh->g_Vertices.buffer, NULL);
    }
    if (mesh->g_Indices.buffer != NULL)
    {
        vkDestroyBuffer(mesh->engineState->device, mesh->g_Indices.buffer, NULL);
    }

    mesh->g_Vertices = vertexbuf;
    mesh->g_Indices = indexbuf;
} 

void RenderMesh(VkCommandBuffer cmnd, Mesh* mesh, Material* mat, mat4 TRS)
{
    MeshParameter p =  {.meshAddress = mesh->g_Address};
    glm_mat4_copy(TRS, p.worldMatrix);
    Material_SetParameter(mat, 0, &p);
    Bind_Material(cmnd, mat);
    vkCmdBindIndexBuffer(cmnd, mesh->g_Indices.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmnd, mesh->indexCount, 1, 0, 0, 0);
}

BufferInfo CreateBuffer(VmaAllocator alloc, size_t size, VmaMemoryUsage mem_property, VkBufferUsageFlags buf_usage)
{
    BufferInfo n = {};

    VkBufferCreateInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bInfo.size = size;
    bInfo.usage = buf_usage;
    VmaAllocationCreateInfo cInfo = {};
    cInfo.usage = mem_property;
    cInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(alloc, &bInfo, &cInfo, &n.buffer, &n.allocation, &n.allocInfo) != VK_SUCCESS)
    {
        printf("!Failed to create buffer of size %u\n", size);
        return (BufferInfo){};
    }
    return n;
}
