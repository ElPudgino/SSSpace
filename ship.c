#include "ship.h"
#include "mesh_gen.h"

void Set_PartTransform(Part* part, Transform* tr)
{
    part->localTransform = *tr;
    //self.mdoeltransform.matrixes[part.matrixindex] = ...
}

uint32_t Get_IndexFromPos(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z)
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

Part* Create_Part(void* structure)
{
    Part* res = (Part*)calloc(1, sizeof(Part));
    res->structure = structure;
    return res;
}

// Block may be bigger than 1 by 1
void Get_LogicBlockCenter(LogicBlock block, vec3 result)
{
    
}

int Has_SpecialRender(LogicBlock block)
{
    return block.blockType & (1 << 23);
}

void Render_SpecialBlock(LogicBlock* block, mat4 prev)
{

}

void Render_Grid(PartStructureGrid* grid, mat4 prev)
{
    assert(grid);
    //printf("Start render grid\n");
    for (int i = 0; i < grid->matCount; i++)
    {
        assert(grid->renderDatas[i]->material);
        assert(grid->renderDatas[i]->mesh);
        Add_InstanceToRender(grid->renderDatas[i], prev);
    }
    //printf("Start render grid logic\n");
    for (int i = 0; i < grid->logicBlockCount; i++)
    {
        if (Has_SpecialRender(grid->logicBlocks[i]))
        {
            Render_SpecialBlock(&grid->logicBlocks[i], prev);
        }        
    }
}

void Render_SimpleMesh(PartStructureSimpleMesh* sm, mat4 prev)
{
    assert(sm);
    Add_InstanceToRender(sm->renderData, prev);
}

void Render_Part(Part* part, mat4 prev)
{
    assert(part);
    //printf("getting part structure\n");
    PartStructureSimpleMesh p = *(PartStructureSimpleMesh*)part->structure;
    //printf("got part structure\n");
    mat4 cur;
    Get_LocalRenderTransformMatrix(&part->localTransform, cur);
    glm_mat4_mul(prev, cur, cur);
    switch (p.structureType)
    {
        case PART_TYPE_GRID:
            Render_Grid((PartStructureGrid*)part->structure, cur);
            break;
        case PART_TYPE_SIMPLE_MESH:
            Render_SimpleMesh((PartStructureSimpleMesh*)part->structure, cur);
            break;
        default:
            break;
    }
    for (int i = 0; i< part->childrenCount; i++)
    {
        Render_Part(&part->children[i], cur);
    }
    //printf("Finish render part\n");
}

void Render_Ship(Ship* ship, mat4 tr)
{
    assert(ship);
    assert(ship->model.rootPart);
    Render_Part(ship->model.rootPart, tr);
}

void _Create_ShipPart(Part* ship, Part* bp)
{
    Set_PartTransform(ship, &bp->localTransform);
    if (bp->childrenCount > 0) ship->children = (Part*)calloc(bp->childrenCount, sizeof(Part));
    ship->childrenCount = bp->childrenCount;
    ship->structure = bp->structure;
    for (int i = 0; i < bp->childrenCount; i++)
    {
        _Create_ShipPart(&ship->children[i],&bp->children[i]);
    }
}

Ship* Create_ShipFromBP(ShipBP* bp)
{
    Ship* res = (Ship*)calloc(1, sizeof(Ship));
    res->BP = bp;
    res->model.rootPart = (Part*)calloc(1, sizeof(Part));

    _Create_ShipPart(res->model.rootPart, bp->model.rootPart);
    return res;
}

void Delete_PartStructure(void* structure)
{
    PartStructureSimpleMesh p = *(PartStructureSimpleMesh*)structure;
    switch (p.structureType)
    {
        case PART_TYPE_GRID:
            Destroy_StructureGrid((PartStructureGrid*)structure);
            break;
        case PART_TYPE_SIMPLE_MESH:
            //
            break;
        default:
            break;
    }
}

void _Delete_ShipPart(Part* part)
{
    for (int i = 0; i < part->childrenCount; i++)
    {
        _Delete_ShipPart(&part->children[i]);
    }
    free(part->children);
}

void Delete_Ship(Ship* ship)
{
    _Delete_ShipPart(ship->model.rootPart);

    free(ship->model.rootPart);
    free(ship);
}

void _Delete_ShipBPPart(Part* part)
{
    for (int i = 0; i < part->childrenCount; i++)
    {
        _Delete_ShipPart(&part->children[i]);
    }
    Delete_PartStructure(part->structure);
    free(part->children);
}

void Delete_ShipBP(ShipBP* bp)
{
    _Delete_ShipBPPart(bp->model.rootPart);

    free(bp->model.rootPart);
    free(bp);
}