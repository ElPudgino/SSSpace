#include "debug.h"


void Debug_ValidateAssert_Mesh(Mesh* mesh)
{
    assert(mesh->indexCount % 3 == 0);
    assert(mesh->vertexCount <= mesh->indexCount);
    for (int i = 0; i < mesh->vertexCount; i++)
    {
        assert(glm_vec3_norm(mesh->vertices[i].position) < 10e5);
    }   
}

void Debug_Print_Mesh(Mesh* mesh)
{
    for (int i = 0; i < mesh->vertexCount; i++)
    {
        Vertex vert = mesh->vertices[i];
        printf("vt %d: p(%f %f %f), n(%f %f %f), uv(%f %f)\n", i, vert.position[0], vert.position[1], vert.position[2], 
            vert.extra, vert.normal[0], vert.normal[1], vert.uv[0], vert.uv[1]);
    }
    for (int i = 0; i < mesh->indexCount; i++)
    {
        printf("ind %d: %d\n", i, mesh->indices[i]);
    }
}