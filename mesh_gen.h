#ifndef MESH_GEN
#define MESH_GEN

#include "ship.h"

enum
{
    SIDE_Xp = 1,
    SIDE_Xn = 2,
    SIDE_Yp = 4,
    SIDE_Yn = 8,
    SIDE_Zp = 16,
    SIDE_Zn = 32
};

typedef struct _BlockModel
{
    Vertex* vertices;
    uint32_t vertexCount;
    uint32_t* indices;
    uint32_t indexCount;
    Material* mat;
} BlockModel;

void Generate_MeshForGrid(PartStructureGrid* grid);

uint32_t _Get_Adjacent(BlockGrid g, uint32_t x, uint32_t y, uint32_t z);

void Add_ModelToPart(PartStructureGrid* grid, BlockModel* bmodel,uint32_t posx,uint32_t posy,uint32_t posz,BlockRotation rot);

PartStructureGrid* Create_PartStructureGrid(EngineState* engineState);

#endif