#include "ship.h"

void Set_OwnPartTransform(Part* part, mat4 TRS)
{
    glm_mat4_copy(TRS, part->localTransform);
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
    printf("Start render grid\n");
    for (int i = 0; i < grid->matCount; i++)
    {
        assert(grid->renderDatas[i]->material);
        assert(grid->renderDatas[i]->mesh);
        Add_InstanceToRender(grid->renderDatas[i], prev);
    }
    printf("Start render grid logic\n");
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
    printf("getting part structure\n");
    PartStructureSimpleMesh p = *(PartStructureSimpleMesh*)part->structure;
    printf("got part structure\n");
    mat4 cur;
    glm_mat4_mul(prev, part->baseLocalTransform, cur);
    glm_mat4_mul(cur, part->localTransform, cur);
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
    printf("Finish render part\n");
}

void Render_Ship(Ship* ship, mat4 tr)
{
    assert(ship);
    assert(ship->model.rootPart);
    Render_Part(ship->model.rootPart, tr);
}