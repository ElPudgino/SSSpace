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

typedef struct _Model
{
    uint64_t ID;
    InstancedRenderData** renderDatas;
    uint32_t matCount;
    float bbsize[3];
} Model;

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

// Any logic is in logic blocks:
// A turret is logic block, which has a special render

typedef struct _LogicBlock
{
    uint32_t data; //offset
    short pos[3]; 
    BlockRotation rot;
    uint32_t defIndex;
} LogicBlock;

typedef enum
{
    SIDE_Xp = 1,
    SIDE_Xn = 2,
    SIDE_Yp = 4,
    SIDE_Yn = 8,
    SIDE_Zp = 16,
    SIDE_Zn = 32
} BlockSide;

typedef struct _BlockModel
{
    Vertex* vertices;
    uint32_t vertexCount;
    uint32_t* indices;
    uint32_t indexCount;
    Material* mat;
} BlockModel;

typedef struct _BlockGrid
{
    Block* array;
    uint32_t x_s;
    uint32_t y_s;
    uint32_t z_s;
} BlockGrid;

typedef struct _PartStructureGrid
{
    uint64_t ID; // todo use a string UUID
    uint32_t userCount;
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
    PartStructureGrid* structure; // shared
    Transform localTransform;
    struct _Part* children;
    uint32_t childrenCount;
} Part;

int Has_SpecialRender(LogicBlock block);

Model* Create_Model();

void AddUpload_ModelTransformArrays(Model* model);

/*! @brief Creates new instance of PartStructureGrid
* Internal arrays are allocated
* @param engineState Main engine state
*/
PartStructureGrid* Create_PartStructureGrid(EngineState* engineState);

void Render_Model(Model* mm, mat4 prev);

void Render_Grid(PartStructureGrid* grid, void* logicblockdata, mat4 prev);

/*! @brief Destroy PartStructureGrid and its arrays
* Frees its internal arrays and invalidates TransformArray of its mesh
* @param grid Target grid
*/
void Destroy_StructureGrid(PartStructureGrid* grid);

void Destroy_Model(Model* structure);

#endif