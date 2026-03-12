#include "tests.h"

ShipBP* shipBp;
Sector* sector;

ShipBP* get_testbp()
{
    return shipBp; 
}

Sector* get_testsector()
{
    return sector;
}

void create_testsector()
{
    sector = Init_Sector();
}

void create_testshipbp(EngineState* engineState)
{
    shipBp = (ShipBP*)calloc(1,sizeof(ShipBP));
    shipBp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState));
    Part* root = shipBp->model.rootPart;
    root->localTransform = (Transform){0.0,0.0,0.0,1.0,0.0,0.0,0.0,0};

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
    Calc_ShipBB(shipBp);
}

