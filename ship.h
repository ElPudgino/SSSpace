#ifndef GAME_SHIP
#define GAME_SHIP

#include "libs.h"
#include "mesh_utils.h"
#include "transform_utils.h"

enum
{
    PART_TYPE_GRID,
    PART_TYPE_SIMPLE_MESH
};

typedef struct _PartStructureSimpleMesh
{
    uint32_t structureType;
    uint32_t ID; 
    InstancedRenderData* renderData;
} PartStructureSimpleMesh;

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
    InstancedRenderData** renderDatas; // Importnant: these arent pointer to the data in TransformArray
                                       // These are separate objects. You are not supposed to modify them after adding a trasform array
                                       // Your copy is just a convenient reference for ID and pointers to mesh and material
    uint32_t matCount;
    uint32_t matCap; 
    LogicBlock* logicBlocks;
    uint32_t logicBlockCount;
} PartStructureGrid;

typedef struct _Part
{
    void* structure; // shared
    Transform localTransform;
    struct _Part* children;
    uint32_t childrenCount;
} Part;

typedef struct _Model
{
    Part* rootPart;
} Model;

// map all parts to an array of matrixes (mapping is persistent for each model)
// when rendering model apply the mapped matrix to the part
// rendering:
// instanced,
// 1 option 
// parts global transforms are stored in 1 array for each unique part
// before rendering for each part write final transform matrix at the end of correspondong array
// every frame this array starts at 0
// another option:
// form a large array of modelTransforms,
// if we have n instances of a model, k matrixes per model
// render mesh1 n times with offset k
// send parent tree indexes to gpu, apply local transforms there
// parent tree is per model, not per instance 

// try 1 option first
// if chunk in which mesh is is invisible just dont add its matrix to the array
// special effects follow the same principle
// just add an array for each special effect
// specific renderers can just add a matrix to one of the arrays to render something
// adding instance data to the array can be done anytime in the frame (need to wait for copy-to-gpu fence though)

// Ship blueprint. Holds shared data like structure of the ship
// Model is stored the same way as in instances but all localTransforms are identity
// To create a new ship basically create new parts and copy structures pointers from BP
// Also cached ship data is stored in BP
typedef struct _ShipBP 
{
    Model model;
} ShipBP;

typedef struct _Ship
{
    ShipBP* BP;
    Model model;
} Ship;

uint32_t Get_IndexFromPos(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z);

Block Get_GridBlock(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z);

void Set_GridBlock(BlockGrid grid, Block block, uint32_t x, uint32_t y, uint32_t z);

Part* Create_Part(void* structure);

int Has_SpecialRender(LogicBlock block);

void Render_Ship(Ship* ship, mat4 tr);

Ship* Create_ShipFromBP(ShipBP* bp);

void Delete_Ship(Ship* ship);

void Delete_ShipBP(ShipBP* bp);

#endif