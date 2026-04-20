#include "ship.h"
#include "mesh_gen.h"
#include "sector.h"
#include "basic_objects.h"
#include "part_table.h"

void Set_PartTransform(Part* part, Transform* tr)
{
    assert(part);
    part->localTransform = *tr;
    //self.mdoeltransform.matrixes[part.matrixindex] = ...
}

inline uint32_t Get_IndexFromPos(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z)
{
    return x + y * grid.x_s + z * grid.x_s * grid.y_s;
}

void Set_GridBlock(BlockGrid grid, Block block, uint32_t x, uint32_t y, uint32_t z)
{
    if (x < 0 || y < 0 || z < 0 || x >= grid.x_s || y >= grid.y_s || z >= grid.z_s) {/*printf("-grid position out of bounds\n");*/ return;}
    grid.array[Get_IndexFromPos(grid, x, y, z)] = block;
}

Block Get_GridBlock(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z)
{
    if (x < 0 || y < 0 || z < 0 || x >= grid.x_s || y >= grid.y_s || z >= grid.z_s) {/*printf("-grid position out of bounds\n");*/ return (Block){0};}
    return grid.array[Get_IndexFromPos(grid, x, y, z)];
}

// Only for creating ship BPs
Part* Create_Part(PartStructureGrid* structure)
{
    assert(structure);
    Part* res = (Part*)calloc(1, sizeof(Part));
    res->structure = structure;
    structure->userCount++;
    return res;
}

void Render_Part(Ship* ship, Part* part, mat4 prev)
{
    assert(ship);
    assert(part);
    //printf("getting part structure\n");
    mat4 cur;
    //mat4 copy;
    Get_LocalRenderTransformMatrix(&part->localTransform, cur);
    //glm_mat4_copy(cur, copy);
    glm_mat4_mul(prev, cur, cur);

    Render_Grid(part->structure, ship->logicBlockData, cur);

    for (int i = 0; i< part->childrenCount; i++)
    {
        Render_Part(ship, &part->children[i], cur);
        //glm_mat4_copy(copy, cur);
    }
    //printf("Finish render part\n");
}

void Render_Ship(Ship* ship, mat4 tr)
{
    assert(ship);
    assert(ship->model.rootPart);
    Render_Part(ship, ship->model.rootPart, tr);
}

// TODO: Take into account rotation constraints on the parts to reduce bb size

void Get_PartStructureBBsize(PartStructureGrid* part, float bb[3])
{
    assert(part);
    float a = 0;
    float b = 0;
    vec3 vec = {};
  
    vec[0] = (float)part->grid.x_s - part->centerOffset[0];
    vec[1] = (float)part->grid.y_s - part->centerOffset[1];
    vec[2] = (float)part->grid.z_s - part->centerOffset[2];
    a = glm_vec3_norm(vec);
    b = glm_vec3_norm(part->centerOffset);
    a = a > b ? a : b;
    bb[0] = a;
    bb[1] = a;
    bb[2] = a;

}

void Get_PartBBsize(Part* p, float s[3])
{
    assert(p);
    s[0] = 0;
    s[1] = 0;
    s[2] = 0;
    vec3 tm;
    vec3 tr;
    for (int i = 0; i < p->childrenCount; i++)
    {
        Get_PartBBsize(&p->children[i], tm);
        tr[0] = (float)p->children[i].localTransform.pos[0];
        tr[1] = (float)p->children[i].localTransform.pos[1];
        tr[2] = (float)p->children[i].localTransform.pos[2];
        glm_vec3_abs(tr, tr);
        glm_vec3_add(tm, tr, tm); 
        s[0] = tm[0] > s[0] ? tm[0] : s[0];
        s[1] = tm[1] > s[1] ? tm[1] : s[1]; 
        s[2] = tm[2] > s[2] ? tm[2] : s[2];  
    }
    Get_PartStructureBBsize(p->structure, tm);
    s[0] = tm[0] > s[0] ? tm[0] : s[0];
    s[1] = tm[1] > s[1] ? tm[1] : s[1]; 
    s[2] = tm[2] > s[2] ? tm[2] : s[2]; 
}

// BB vector is in local coords
void Calc_ShipBB(ShipBP* bp)
{
    assert(bp);
    Get_PartBBsize(bp->model.rootPart, bp->BB);
}

float Get_ShipBBsize(Ship* ship)
{
    assert(ship);
    return glm_vec3_norm(ship->BP->BB);
}

void _Create_ShipPart(Part* ship, Part* bp)
{
    assert(ship);
    assert(bp);
    Set_PartTransform(ship, &bp->localTransform);
    if (bp->childrenCount > 0) ship->children = (Part*)calloc(bp->childrenCount, sizeof(Part));
    ship->childrenCount = bp->childrenCount;
    ship->structure = bp->structure;
    bp->structure->userCount++;
    for (int i = 0; i < bp->childrenCount; i++)
    {
        _Create_ShipPart(&ship->children[i],&bp->children[i]);
    }
}

// Logic blocks are only stored in BP
// Their instance data is stored in ship instances
// void* data in logicblocks are offsets, added to the start of instance data array
void ShipBP_Add_LogicBlock(ShipBP* bp, PartStructureGrid* part, uint32_t blockID, short pos[3], BlockRotation rot)
{
    assert(bp);
    assert(part);
    assert(blockID); // ID 0 is reserved as invalid
    const LogicBlockDef* def = Get_LogicBlockDef(blockID);
    if (part->logicBlockCount == part->logicBlockCap)
    {
        part->logicBlocks = (LogicBlock*)realloc(part->logicBlocks, 2*part->logicBlockCap*sizeof(LogicBlock));
        part->logicBlockCap *= 2;
    }
    LogicBlock b = (LogicBlock){.data = def->instanceDataSize, .rot = rot,.defIndex = def->ID};
    b.pos[0] = pos[0];
    b.pos[1] = pos[1];
    b.pos[2] = pos[2];
    part->logicBlocks[part->logicBlockCount] = b;
    part->logicBlockCount++;
    bp->logicBlockDataLength += def->instanceDataSize;
}
// Slightly cursed, but the main idea is here
void* Get_LogicBlockData(Ship* ship, LogicBlock block)
{
    assert(ship);
    return (void*)((char*)ship->logicBlockData + block.data);
}

// Create an array to hold logicblock data from the entire ship
Ship* Create_ShipFromBP(ShipBP* bp)
{
    assert(bp);
    Ship* res = (Ship*)calloc(1, sizeof(Ship));
    res->BP = bp;
    res->model.rootPart = (Part*)calloc(1, sizeof(Part));
    res->rb = bp->rb;
    res->logicBlockData = calloc(1, bp->logicBlockDataLength); 
    assert(res->logicBlockData);

    _Create_ShipPart(res->model.rootPart, bp->model.rootPart);

    res->rb.root = &res->model.rootPart->localTransform;
    printf("Created ship from bp\n");
    return res;
}

void _Delete_ShipPart(Part* part)
{
    assert(part);
    part->structure->userCount--;
    for (int i = 0; i < part->childrenCount; i++)
    {
        _Delete_ShipPart(&part->children[i]);
    }
    free(part->children);
}

void Delete_Ship(Ship* ship)
{
    assert(ship);
    _Delete_ShipPart(ship->model.rootPart);

    free(ship->model.rootPart);
    free(ship);
}

void _Delete_ShipBPPart(Part* part)
{
    assert(part);
    part->structure->userCount--;
    for (int i = 0; i < part->childrenCount; i++)
    {
        _Delete_ShipPart(&part->children[i]);
    }
    // A ship may have mutiple parts of the same type
    // Cant delete them like this
    // Delete_PartStructure(part->structure);
    free(part->children);
}

void Delete_ShipBP(ShipBP* bp)
{
    assert(bp);
    _Delete_ShipBPPart(bp->model.rootPart);

    free(bp->model.rootPart);
    free(bp);
}
