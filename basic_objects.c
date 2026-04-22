#include "basic_objects.h"
#include "assets.h"
#include "mesh_utils.h"
#include "part_table.h"

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

Model* Create_Model()
{
    Model* m = (Model*)calloc(1, sizeof(Model));
    return m;
}

PartStructureGrid* Create_PartStructureGrid(EngineState* engineState)
{
    PartStructureGrid* res = (PartStructureGrid*)calloc(1, sizeof(PartStructureGrid));
    //res->matCap = 1;
    //res->renderDatas = (InstancedRenderData**)calloc(1, sizeof(InstancedRenderData*));
    res->ID = (uint64_t)rand() + (((uint64_t)rand()) << 32); // TODO: use a proper UUID. Chance of collision is already abysmal though
    res->engineState = engineState;
    PartTable_Set_Part(res);
    return res;
}

PartStructureGrid* Create_PartStructureGrid_ID(EngineState* engineState, uint64_t id)
{
    PartStructureGrid* res = (PartStructureGrid*)calloc(1, sizeof(PartStructureGrid));
    //res->matCap = 1;
    //res->renderDatas = (InstancedRenderData**)calloc(1, sizeof(InstancedRenderData*));
    res->ID = id;
    res->engineState = engineState;
    PartTable_Set_Part(res);
    return res;
}

void AddUpload_ModelTransformArrays(Model* mm)
{
    assert(mm);
    assert(mm->renderDatas);
    for (int i = 0; i < mm->matCount; i++)
    {
        Mesh_UploadData(mm->renderDatas[i]->mesh);
        Add_TransformArray(mm->renderDatas[i]);
    }
}

void Render_Grid(PartStructureGrid* grid, void* logicblockdata, mat4 prev)
{
    assert(grid);
    assert(grid->renderDatas);
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

void Render_Model(Model* mm, mat4 prev)
{
    assert(mm);
    assert(mm->renderDatas);
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

void Destroy_Model(Model* model)
{
    for (int i = 0; i < model->matCount; i++)
    {
        Destroy_TransformArray(model->renderDatas[i]);
        free(model->renderDatas[i]);
    }
    free(model->renderDatas);
    free(model);
}
