#ifndef BASIC_OBJECTS
#define BASIC_OBJECTS

#include "libs.h"
#include "mesh_utils.h"
#include "transform_utils.h"

enum
{
    PART_TYPE_GRID,
    PART_TYPE_SIMPLE_MESH,
    PART_TYPE_MULTI_MESH
};

typedef struct _PartStructureSimpleMesh
{
    uint32_t structureType;
    uint32_t ID; 
    InstancedRenderData* renderData;
    float bbsize[3];
} PartStructureSimpleMesh;

typedef struct _PartStructureMultiMesh
{
    uint32_t structureType;
    uint32_t ID;
    InstancedRenderData** renderDatas;
    uint32_t matCount;
    float bbsize[3];
} PartStructureMultiMesh;

// 24-th bit is whether this block has special rendering (only for logic blocks)
typedef uint32_t BlockType;

// last byte of blocktype is rotation 
typedef char BlockRotation;

typedef struct _Multiblock
{
    BlockType blockType;
    uint32_t x_size;
    uint32_t y_size;
    uint32_t z_size;
} Multiblock;

typedef struct _Block
{
    BlockType blockType;
} Block;

typedef struct _LogicBlock
{
    BlockType blockType;
    void* data;
    uint32_t pos[3]; 
} LogicBlock;

typedef struct _BlockGrid
{
    Block* array;
    uint32_t x_s;
    uint32_t y_s;
    uint32_t z_s;
} BlockGrid;

typedef struct _PartStructureGrid
{
    uint32_t structureType;
    uint32_t ID; 
    EngineState* engineState;
    BlockGrid grid;
    float centerOffset[3];
    InstancedRenderData** renderDatas; // Important: these arent pointer to the data in TransformArray
                                       // These are separate objects. You are not supposed to modify them after adding a trasform array
                                       // Your copy is just a convenient reference for ID and pointers to mesh and material
    uint32_t matCount;
    uint32_t matCap; 
    LogicBlock* logicBlocks;
    uint32_t logicBlockCount;
    uint32_t logicBlockCap;
} PartStructureGrid;

typedef struct _Part
{
    void* structure; // shared
    Transform localTransform;
    struct _Part* children;
    uint32_t childrenCount;
} Part;

int Has_SpecialRender(LogicBlock* block);

PartStructureSimpleMesh* Create_PartStructureSimpleMesh();

PartStructureMultiMesh* Create_PartStructureMultiMesh();

/*! @brief Creates new instance of PartStructureGrid
* Internal arrays are allocated
* @param engineState Main engine state
*/
PartStructureGrid* Create_PartStructureGrid(EngineState* engineState);

void Render_SimpleMesh(PartStructureSimpleMesh* sm, mat4 prev);

void Render_MultiMesh(PartStructureMultiMesh* mm, mat4 prev);

void Render_Grid(PartStructureGrid* grid, mat4 prev);

/*! @brief Destroy PartStructureGrid and its arrays
* Frees its internal arrays and invalidates TransformArray of its mesh
* @param grid Target grid
*/
void Destroy_StructureGrid(PartStructureGrid* grid);

#endif