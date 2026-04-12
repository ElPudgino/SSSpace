#include "debug.h"
#include "logger.h"

#define MAX_VALID_VERTEX_DIST 10e5

int Debug_Validate_Mesh(Mesh* mesh)
{
    int res = 0;
    if (mesh->indexCount % 3 != 0) {res = 1; LOG_TEXT("!Mesh Index count not a multiple of 3\n");}
    if (mesh->vertexCount > mesh->indexCount) {res = 1; LOG_TEXT("!Mesh has more vertices than indices\n");}
    for (int i = 0; i < mesh->vertexCount; i++)
    {
        if (glm_vec3_norm(mesh->vertices[i].position) >= MAX_VALID_VERTEX_DIST) 
        {res = 1; LOG_INFO("!Mesh vertex to far from center (dist > %lf)\n", MAX_VALID_VERTEX_DIST); break;};
    }   
    return res;
}

void Debug_Print_Mesh(Mesh* mesh)
{
    for (int i = 0; i < mesh->vertexCount; i++)
    {
        Vertex vert = mesh->vertices[i];
        LOG_INFO("vt %d: p(%f %f %f), n(%f %f %f), uv(%f %f)\n", i, vert.position[0], vert.position[1], vert.position[2], 
            vert.extra, vert.normal[0], vert.normal[1], vert.uv[0], vert.uv[1]);
    }
    for (int i = 0; i < mesh->indexCount; i++)
    {
        LOG_INFO("ind %d: %d\n", i, mesh->indices[i]);
    }
}