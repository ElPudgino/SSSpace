#include "blocks.h"

BlockModel* cube;

void _Init_Cube()
{
    cube = (BlockModel*)calloc(1, sizeof(BlockModel));
    cube->vertices = (Vertex*)calloc(24, sizeof(Vertex)); // max size
    cube->indices = (uint32_t*)calloc(36, sizeof(uint32_t));
    assert(GetMaterial_Test());
    cube->mat = GetMaterial_Test();
}

void Destroy_BlockModels()
{
    free(cube->indices);
    free(cube->vertices);
    free(cube);
}

BlockModel* Get_CubeModel(uint32_t sides)
{
    // Reduce vertex count to 8 when everything else works
    cube->indexCount = 0;
    cube->vertexCount = 0;

    if ((~sides) & SIDE_Xp)
    {
        cube->vertices[0+cube->vertexCount] = (Vertex){1.0,0.0,0.0};
        cube->vertices[1+cube->vertexCount] = (Vertex){1.0,0.0,1.0};
        cube->vertices[2+cube->vertexCount] = (Vertex){1.0,1.0,0.0};
        cube->vertices[3+cube->vertexCount] = (Vertex){1.0,1.0,1.0};
        cube->indices[0+cube->indexCount] = 0 + cube->vertexCount;
        cube->indices[1+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[2+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[3+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[4+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[5+cube->indexCount] = 3 + cube->vertexCount;
        cube->indexCount += 6;
        cube->vertexCount += 4;
    }
    if ((~sides) & SIDE_Xn)
    {
        cube->vertices[0+cube->vertexCount] = (Vertex){0.0,0.0,0.0};
        cube->vertices[1+cube->vertexCount] = (Vertex){0.0,0.0,1.0};
        cube->vertices[2+cube->vertexCount] = (Vertex){0.0,1.0,0.0};
        cube->vertices[3+cube->vertexCount] = (Vertex){0.0,1.0,1.0};
        cube->indices[0+cube->indexCount] = 0 + cube->vertexCount;
        cube->indices[1+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[2+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[3+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[4+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[5+cube->indexCount] = 3 + cube->vertexCount;
        cube->indexCount += 6;
        cube->vertexCount += 4;
    }
    if ((~sides) & SIDE_Yp)
    {
        cube->vertices[0+cube->vertexCount] = (Vertex){0.0,1.0,0.0};
        cube->vertices[1+cube->vertexCount] = (Vertex){1.0,1.0,0.0};
        cube->vertices[2+cube->vertexCount] = (Vertex){0.0,1.0,1.0};
        cube->vertices[3+cube->vertexCount] = (Vertex){1.0,1.0,1.0};
        cube->indices[0+cube->indexCount] = 0 + cube->vertexCount;
        cube->indices[1+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[2+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[3+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[4+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[5+cube->indexCount] = 3 + cube->vertexCount;
        cube->indexCount += 6;
        cube->vertexCount += 4;
    }
    if ((~sides) & SIDE_Yn)
    {
        cube->vertices[0+cube->vertexCount] = (Vertex){0.0,0.0,0.0};
        cube->vertices[1+cube->vertexCount] = (Vertex){1.0,0.0,0.0};
        cube->vertices[2+cube->vertexCount] = (Vertex){0.0,0.0,1.0};
        cube->vertices[3+cube->vertexCount] = (Vertex){1.0,0.0,1.0};
        cube->indices[0+cube->indexCount] = 0 + cube->vertexCount;
        cube->indices[1+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[2+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[3+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[4+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[5+cube->indexCount] = 3 + cube->vertexCount;
        cube->indexCount += 6;
        cube->vertexCount += 4;
    }
    if ((~sides) & SIDE_Zp)
    {
        cube->vertices[0+cube->vertexCount] = (Vertex){0.0,0.0,1.0};
        cube->vertices[1+cube->vertexCount] = (Vertex){1.0,0.0,1.0};
        cube->vertices[2+cube->vertexCount] = (Vertex){0.0,1.0,1.0};
        cube->vertices[3+cube->vertexCount] = (Vertex){1.0,1.0,1.0};
        cube->indices[0+cube->indexCount] = 0 + cube->vertexCount;
        cube->indices[1+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[2+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[3+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[4+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[5+cube->indexCount] = 3 + cube->vertexCount;
        cube->indexCount += 6;
        cube->vertexCount += 4;
    }
    if ((~sides) & SIDE_Zn)
    {
        cube->vertices[0+cube->vertexCount] = (Vertex){0.0,0.0,0.0};
        cube->vertices[1+cube->vertexCount] = (Vertex){1.0,0.0,0.0};
        cube->vertices[2+cube->vertexCount] = (Vertex){0.0,1.0,0.0};
        cube->vertices[3+cube->vertexCount] = (Vertex){1.0,1.0,0.0};
        cube->indices[0+cube->indexCount] = 0 + cube->vertexCount;
        cube->indices[1+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[2+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[3+cube->indexCount] = 2 + cube->vertexCount;
        cube->indices[4+cube->indexCount] = 1 + cube->vertexCount;
        cube->indices[5+cube->indexCount] = 3 + cube->vertexCount;
        cube->indexCount += 6;
        cube->vertexCount += 4;
    }

    return cube;
}