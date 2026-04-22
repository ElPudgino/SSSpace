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
    shipBp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState), NULL);
    Part* root = shipBp->model.rootPart;
    root->localTransform = (Transform){0.0,0.0,0.0,1.0,0.0,0.0,0.0,0};
    PartStructureGrid* grid = NULL;

    root->children = (Part*)calloc(2, sizeof(Part));
    root->childrenCount = 2;
    root->children[0].localTransform = (Transform){6.0,4.5,0.0,1.0,0.0,0.0,0.0,&root->localTransform};
    root->children[1].localTransform = (Transform){0.0,-9.0,10.0,1.0,0.0,0.0,0.0,&root->localTransform};
    

    Create_Part(Create_PartStructureGrid(engineState), root->children);
    grid = root->children[0].structure;
    grid->grid.array = (Block*)calloc(4*4*6, sizeof(Block));
    grid->grid.x_s = 4;
    grid->grid.y_s = 4;
    grid->grid.z_s = 6;
    grid->centerOffset[0] = 2;
    grid->centerOffset[1] = 2;
    grid->centerOffset[2] = 1;

    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            for (int z = 0; z < 6; z++)
            {
                if ((x+y+z) % 2 == 0)
                {
                    Set_GridBlock(grid->grid, (Block){1}, x, y, z);
                }
            }
        }
    }
    Generate_MeshForGrid(grid);

    Create_Part(Create_PartStructureGrid(engineState), root->children + 1);
    grid = root->children[1].structure;
    grid->grid.array = (Block*)calloc(2*1*15, sizeof(Block));
    grid->grid.x_s = 2;
    grid->grid.y_s = 1;
    grid->grid.z_s = 15;
    grid->centerOffset[0] = 1;
    grid->centerOffset[1] = 0;
    grid->centerOffset[2] = 4;

    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 1; y++)
        {
            for (int z = 0; z < 15; z++)
            {
                Set_GridBlock(grid->grid, (Block){1}, x, y, z);
            }
        }
    }
    Generate_MeshForGrid(grid);

    grid = root->structure;
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

    Grid_Add_LogicBlock(grid, 1, (short[3]){0, 0, 13}, 0);

    glm_mat3_copy(GLM_MAT3_IDENTITY, shipBp->rb.baseInertiaTensor);
    glm_mat3_scale(shipBp->rb.baseInertiaTensor, 10000);
    glm_mat3_inv(shipBp->rb.baseInertiaTensor, shipBp->rb.invBaseInertiaTensor);
    shipBp->rb.baseMass = 1000.0;
    //shipBp->rb.baseCenterOfMass = 

    Generate_MeshForGrid(grid);
    Calc_ShipBB(shipBp);
    ShipBP_Init_LogicBlocks(shipBp);
}
