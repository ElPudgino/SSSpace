#include "basic_objects.h"

// Block may be bigger than 1 by 1
// result in local coords
void Get_LogicBlockCenter(LogicBlock* block, vec3 result)
{
    
}

// If block has special render dont add it to ship mesh; 
// Render it separately instead
int Has_SpecialRender(LogicBlock* block)
{
    return block->blockType & (1 << 23);
}

void Render_SpecialBlock(LogicBlock* block, mat4 prev)
{

}

PartStructureMultiMesh* Create_PartStructureMultiMesh()
{
    PartStructureMultiMesh* m = (PartStructureMultiMesh*)calloc(1, sizeof(PartStructureMultiMesh));
    m->structureType = PART_TYPE_MULTI_MESH;   
    return m;
}

PartStructureGrid* Create_PartStructureGrid(EngineState* engineState)
{
    PartStructureGrid* res = (PartStructureGrid*)calloc(1, sizeof(PartStructureGrid));
    res->matCap = 1;
    res->renderDatas = (InstancedRenderData**)calloc(1, sizeof(InstancedRenderData*));
    res->logicBlocks = (LogicBlock*)calloc(8, sizeof(LogicBlock));
    res->logicBlockCap = 8;
    res->structureType = PART_TYPE_GRID;
    res->engineState = engineState;
    return res;
}

PartStructureSimpleMesh* Create_PartStructureSimpleMesh()
{
    PartStructureSimpleMesh* m = (PartStructureSimpleMesh*)calloc(1, sizeof(PartStructureSimpleMesh));
    m->structureType = PART_TYPE_SIMPLE_MESH;
    return m;
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
        if (Has_SpecialRender(&grid->logicBlocks[i]))
        {
            Render_SpecialBlock(&grid->logicBlocks[i], prev);
        }        
    }
}

void Render_SimpleMesh(PartStructureSimpleMesh* sm, mat4 prev)
{
    assert(sm);
    assert(sm->structureType == PART_TYPE_SIMPLE_MESH);
    Add_InstanceToRender(sm->renderData, prev);
}

void Render_MultiMesh(PartStructureMultiMesh* mm, mat4 prev)
{
    assert(mm);
    assert(mm->renderDatas);
    assert(mm->structureType == PART_TYPE_MULTI_MESH);
    for (int i = 0; i < mm->matCount; i++)
    {
        Add_InstanceToRender(mm->renderDatas[i], prev);
    }
}

void Destroy_StructureGrid(PartStructureGrid* grid)
{
    free(grid->grid.array);
    if (grid->logicBlocks) free(grid->logicBlocks);
    for (int i = 0; i < grid->matCount; i++)
    {
        Destroy_TransformArray(grid->renderDatas[i]);
    }

    if (grid->renderDatas) 
    {
        for (int i = 0; i < grid->matCount; i++)
        {
            free(grid->renderDatas[i]);
        }
        free(grid->renderDatas);
    }
    free(grid);
}

