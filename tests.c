#include "tests.h"

ShipBP* shipBp;

void _Create_ShipPart(Part* ship, Part* bp)
{
    glm_mat4_copy(bp->baseLocalTransform, ship->baseLocalTransform);
    glm_mat4_copy(bp->localTransform, ship->localTransform);
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

ShipBP* get_testbp()
{
    return shipBp; 
}

void create_testshipbp(EngineState* engineState)
{
    shipBp = (ShipBP*)calloc(1,sizeof(ShipBP));
    shipBp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState));
    Part* root = shipBp->model.rootPart;
    glm_mat4_copy(GLM_MAT4_IDENTITY, root->baseLocalTransform);
    glm_mat4_copy(GLM_MAT4_IDENTITY, root->localTransform);

    PartStructureGrid* grid = (PartStructureGrid*)root->structure;
    grid->grid.array = (Block*)calloc(11*11*11, sizeof(Block));
    grid->grid.x_s = 11;
    grid->grid.y_s = 11;
    grid->grid.z_s = 11;
    grid->centerOffset[0] = 5.5;
    grid->centerOffset[1] = 5.5;
    grid->centerOffset[2] = 5.5;

    for (int x = 0; x < 11; x++)
    {
        for (int y = 0; y < 11; y++)
        {
            for (int z = 0; z < 11; z++)
            {
                if (x+y+z < 11)
                {
                    Set_GridBlock(grid->grid, (Block){1}, x, y, z);
                }
            }
        }
    }

    Generate_MeshForGrid(grid);
}

