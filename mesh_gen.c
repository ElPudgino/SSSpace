#include "mesh_gen.h"
#include "transform_utils.h"
#include "ship.h"
#include "blocks.h"

BlockModel* GetBlockModel(uint32_t type, uint32_t visible_sides)
{
    uint32_t t = type & 0x007FFFFF;
    switch (t)
    {
        case 0:
            return NULL;
            break;
        case 1: // Solid cube
            return Get_CubeModel(visible_sides);
            break;
        
        default:
            return NULL;
            break;
    }
}

BlockRotation Get_BlockRotation(BlockType type)
{
    return type >> 24; // last byte is rotation
}

// rotate around the center of the corner block (so around 0.5, 0.5, 0.5)
// probably 3 bits for direction of +x and 3 bits for dir of +y

void Rotate_BlockVertex(Vertex* vert, BlockRotation rot)
{

}

void _addArrays(Mesh* mesh, BlockModel* bmodel, float x, float y, float z, BlockRotation rot)
{
    if (mesh->indexCap == 0)
    {
        mesh->indices = (uint32_t*)calloc(6, sizeof(uint32_t));
        mesh->indexCap = 6;
    }
    uint32_t prevvertexcount = mesh->vertexCount;
    
    for (int i = 0; i < bmodel->indexCount; i++)
    {
        if (mesh->indexCount == mesh->indexCap)
        {
            mesh->indices = (uint32_t*)realloc(mesh->indices, mesh->indexCap * 2 * sizeof(uint32_t));
            mesh->indexCap *= 2;
        }
        mesh->indices[mesh->indexCount] = bmodel->indices[i] + prevvertexcount; // indices must point to new verts at the end of the array
        mesh->indexCount++;
    }

    if (mesh->vertexCap == 0)
    {
        mesh->vertices = (Vertex*)calloc(6, sizeof(Vertex));
        mesh->vertexCap = 6;
    }
    
    vec3 offset;
    offset[0] = x;
    offset[1] = y;
    offset[2] = z;
    for (int i = 0; i < bmodel->vertexCount; i++)
    {
        if (mesh->vertexCount == mesh->vertexCap)
        {
            mesh->vertices = (Vertex*)realloc(mesh->vertices, mesh->vertexCap * 2 * sizeof(Vertex));
            mesh->vertexCap *= 2;
        }
        Vertex vert = bmodel->vertices[i];
        Rotate_BlockVertex(&bmodel->vertices[i],rot);
        glm_vec3_add(vert.position,offset,vert.position);
        mesh->vertices[mesh->vertexCount] = vert;
        mesh->vertexCount++;
    }    
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

void Set_GridToStructure(PartStructureGrid* structure, BlockGrid grid)
{
    structure->grid = grid;
}

void Add_ModelToPart(PartStructureGrid* grid, BlockModel* bmodel,uint32_t posx,uint32_t posy,uint32_t posz,BlockRotation rot)
{
    if (bmodel == NULL) return; 
    for (int i = 0; i < grid->matCount;i++)
    {
        if (grid->renderDatas[i]->material->ID == bmodel->mat->ID)
        {
            grid->renderDatas[i]->updateNeeded = 1;
            _addArrays(grid->renderDatas[i]->mesh, bmodel, (float)posx-grid->centerOffset[0], (float)posy-grid->centerOffset[1], (float)posz-grid->centerOffset[2], rot);
            return;
        }
    }
    if (grid->matCap == grid->matCount)
    {
        grid->renderDatas = (InstancedRenderData**)realloc(grid->renderDatas, sizeof(InstancedRenderData*)*grid->matCap*2);
        grid->matCap *= 2;
    }
    InstancedRenderData* d = Create_InstanceData();
    assert(d);
    assert(bmodel->mat);
    d->updateNeeded = 1;
    d->material = bmodel->mat;
    d->mesh = Create_Mesh(grid->engineState);
    d->mesh->deleteWithRenderInstance = 1;
    assert(d->mesh);
    grid->renderDatas[grid->matCount] = d;
    _addArrays(grid->renderDatas[grid->matCount]->mesh, bmodel, (float)posx-grid->centerOffset[0], (float)posy-grid->centerOffset[1], (float)posz-grid->centerOffset[2], rot);
    grid->matCount++;
}

BlockSide Get_Adjacent(BlockGrid g, uint32_t x, uint32_t y, uint32_t z)
{
    int res = 0;
    if (Get_GridBlock(g, x+1, y ,z).blockType != 0) res = res | SIDE_Xp;
    if (Get_GridBlock(g, x-1, y ,z).blockType != 0) res = res | SIDE_Xn;
    if (Get_GridBlock(g, x, y+1 ,z).blockType != 0) res = res | SIDE_Yp;
    if (Get_GridBlock(g, x, y-1 ,z).blockType != 0) res = res | SIDE_Yn;
    if (Get_GridBlock(g, x, y ,z+1).blockType != 0) res = res | SIDE_Zp;
    if (Get_GridBlock(g, x, y ,z-1).blockType != 0) res = res | SIDE_Zn;
    return (BlockSide)res;
}

void Generate_MeshForGrid(PartStructureGrid* grid)
{
    assert(grid);
    // TODO: cleanup transform arrays when changing or deleting structures

    BlockGrid g = grid->grid;
    printf("Started adding blocks\n");
    for (int x = 0; x < g.x_s; x++)
    {
        for (int y = 0; y < g.y_s; y++)
        {
            for (int z = 0; z < g.z_s; z++)
            {
                Block b = Get_GridBlock(g, x, y, z);
                Add_ModelToPart(grid, GetBlockModel(b.blockType, Get_Adjacent(g, x ,y ,z)), x,y,z, Get_BlockRotation(b.blockType));
            }
        }
    }

    printf("Started adding logic blocks\n");
    for (int i = 0; i < grid->logicBlockCount;i++)
    {
        LogicBlock b = grid->logicBlocks[i];
        if (!Has_SpecialRender(&b))
        {
            Add_ModelToPart(grid, GetBlockModel(b.blockType, Get_Adjacent(g, b.pos[0] ,b.pos[1] ,b.pos[2])),b.pos[0],b.pos[1],b.pos[2], Get_BlockRotation(b.blockType));
        }
    }

    printf("Uploading meshes, %d\n", grid->matCount);
    for (int i = 0; i < grid->matCount; i++)
    {
        Add_TransformArray(grid->renderDatas[i]);
        Mesh_UploadData(grid->renderDatas[i]->mesh);
    }
    printf("Done generating mesh for grid\n");
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