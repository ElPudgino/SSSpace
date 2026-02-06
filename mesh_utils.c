#include "mesh_utils.h"

Mesh* Create_Mesh(EngineState* engineState)
{
    assert(engineState);
    Mesh* mesh = (Mesh*)calloc(1, sizeof(Mesh));
    mesh->engineState = engineState;
    return mesh;
}

void Mesh_SetVertices(Mesh* mesh, Vertex* vertices, uint32_t count)
{
    assert(mesh);
    assert(vertices);
    mesh->vertexCount = count;
    mesh->vertices = vertices;
}

void Mesh_SetIndices(Mesh* mesh, uint32_t* indices, uint32_t count)
{
    assert(mesh);
    assert(indices);
    mesh->indexCount = count;
    mesh->indices = indices;
}

void Destroy_Mesh(Mesh* mesh)
{
    assert(mesh);
    if (mesh->indices) free(mesh->indices);
    if (mesh->vertices) free(mesh->vertices);
    if (mesh->g_Indices.buffer)
    {
        vmaDestroyBuffer(mesh->engineState->allocator, mesh->g_Indices.buffer, mesh->g_Indices.allocation);
    }
    if (mesh->g_Vertices.buffer)
    {
        vmaDestroyBuffer(mesh->engineState->allocator, mesh->g_Vertices.buffer, mesh->g_Vertices.allocation);
    }
}

void Mesh_UploadData(Mesh* mesh)
{
    assert(mesh);
    assert(mesh->indices);
    assert(mesh->vertices);
    uint32_t ind = mesh->indexCount * sizeof(uint32_t);
    uint32_t ver = mesh->vertexCount * sizeof(Vertex);
    uint32_t tem = ind + ver;

    BufferInfo temp = CreateBuffer(mesh->engineState->allocator, tem, 
        VMA_MEMORY_USAGE_CPU_ONLY, 
        (VmaAllocationCreateFlagBits)(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT ), 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    printf("Created a temp buffer\n");
    BufferInfo vertexbuf = CreateBuffer(mesh->engineState->allocator, ver, 
        VMA_MEMORY_USAGE_CPU_ONLY,
        (VmaAllocationCreateFlagBits)0, 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    BufferInfo indexbuf = CreateBuffer(mesh->engineState->allocator, ind, 
        VMA_MEMORY_USAGE_CPU_ONLY,
        (VmaAllocationCreateFlagBits)0,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    printf("Created buffers\n");
    // Get vertex array gpu pointer
    // We dont need index array pointer though
    VkBufferDeviceAddressInfo info = {.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = vertexbuf.buffer};
    mesh->g_Address = vkGetBufferDeviceAddress(mesh->engineState->device, &info);

    printf("Got buffer device address\n");
    // Map gpu memory with temp buffer to cpu array 

    vmaCopyMemoryToAllocation(mesh->engineState->allocator, mesh->vertices, temp.allocation, 0, ver);
    vmaCopyMemoryToAllocation(mesh->engineState->allocator, mesh->indices, temp.allocation, ver, ind);
    printf("Copied data\n");
    
    ImmediateCommand IC = Command_Immediate_Begin(mesh->engineState->device, mesh->engineState->commandsHandle.miscUpdateBuffer, mesh->engineState->queueHandles._Graphics);
    
    VkBufferCopy vertexCopy = {};
    vertexCopy.dstOffset = 0;
    vertexCopy.srcOffset = 0;
    vertexCopy.size = ver;

    vkCmdCopyBuffer(IC.buffer, temp.buffer, vertexbuf.buffer, 1, &vertexCopy);

    VkBufferCopy indexCopy = {};
    indexCopy.dstOffset = 0;
    indexCopy.srcOffset = ver;
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

    //uint32_t* testarr = (uint32_t*)calloc(6, sizeof(uint32_t)); 
    //vmaCopyAllocationToMemory(mesh->engineState->allocator, indexbuf.allocation, 0, testarr, ind);
    //printf("vert: %d %d %d\n", testarr[0], testarr[1], testarr[2]);
} 

void RenderMeshesInstanced(VkCommandBuffer cmnd, Mesh* mesh, Material* mat)
{
    assert(mesh);
    assert(mat);

    MeshParameter p =  {.meshAddress = mesh->g_Address};
    Material_SetParameter(mat, 0, &p);
    Bind_Material(cmnd, mat);
    vkCmdBindIndexBuffer(cmnd, mesh->g_Indices.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmnd, mesh->indexCount, 1, 0, 0, 0);
}
