#include "basic_objects.h"
#include "assets.h"
#include "mesh_utils.h"

// Block may be bigger than 1 by 1
// result in local coords
void Get_LogicBlockCenter(LogicBlock block, vec3 result)
{
    
}

// If block has special render dont add it to ship mesh; 
// Render it separately instead
int Has_SpecialRender(LogicBlock block)
{
    return Get_LogicBlockDef(block.defIndex)->render != NULL;
}

void Render_SpecialBlock(void* shipdata, LogicBlock block, mat4 prev)
{
    assert(shipdata);
    assert(block.defIndex); // index 0 is reserved as invalid
    const LogicBlockDef* def = Get_LogicBlockDef(block.defIndex);
    assert(def->ID == block.defIndex);
    assert(def->render);
    assert(def->staticData);
    def->render((void*)((char*)shipdata+block.data), def->staticData, prev);
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

void AddUpload_ModelTransformArrays(void* model)
{
    assert(model);
    PartStructureSimpleMesh* sm = (PartStructureSimpleMesh*)model;
    if (sm->structureType == PART_TYPE_SIMPLE_MESH)
    {
        Mesh_UploadData(sm->renderData->mesh);
        Add_TransformArray(sm->renderData);
    }
    else if (sm->structureType == PART_TYPE_MULTI_MESH)
    {
        PartStructureMultiMesh* mm = (PartStructureMultiMesh*)model;
        for (int i = 0; i < mm->matCount; i++)
        {
            Mesh_UploadData(mm->renderDatas[i]->mesh);
            Add_TransformArray(mm->renderDatas[i]);
        }
    }
    else
    {
        assert((printf("!!Called Add_ModelTransformArrays with wrong model type\n"),0));
    } 
}

void Render_Grid(PartStructureGrid* grid, void* logicblockdata, mat4 prev)
{
    assert(grid);
    assert(logicblockdata);
    //printf("Start render grid\n");
    for (int i = 0; i < grid->matCount; i++)
    {
        assert(grid->renderDatas[i]->material);
        assert(grid->renderDatas[i]->mesh);
        Add_InstanceToRender(grid->renderDatas[i], prev);
    }
    //printf("Start render grid logic\n");
    mat4 copy;
    for (int i = 0; i < grid->logicBlockCount; i++)
    {
        LogicBlock b = grid->logicBlocks[i];
        if (Has_SpecialRender(b))
        {
            glm_mat4_mul(prev,(mat4){{1,0,0,0},{0,1,0,0},{0,0,1,0},{(float)b.pos[0]-grid->centerOffset[0],(float)b.pos[1]-grid->centerOffset[1],(float)b.pos[2]-grid->centerOffset[2],1}},copy);
            Render_SpecialBlock(logicblockdata ,b, copy);
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
    assert(grid);
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

