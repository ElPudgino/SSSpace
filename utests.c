#include "sector.h"
#include "ship.h"
#include "mesh_gen.h"
#include "blocks.h"
#include "debug.h"
#include "logger.h"
#include "bploader.h"

int ShipBP1_Test(EngineState* engineState, ShipBP** bp)
{
    int res = 0;

    ShipBP* shipBp = (ShipBP*)calloc(1,sizeof(ShipBP));
    shipBp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState));
    Part* root = shipBp->model.rootPart;
    root->localTransform = (Transform){0.0,0.0,0.0,1.0,0.0,0.0,0.0,0};

    PartStructureGrid* grid = root->structure;
    grid->grid.array = (Block*)calloc(11*6*11, sizeof(Block));
    grid->grid.x_s = 11;
    grid->grid.y_s = 6;
    grid->grid.z_s = 11;
    grid->centerOffset[0] = 5.5;
    grid->centerOffset[1] = 3.0;
    grid->centerOffset[2] = 5.5;

    for (int x = 0; x < 11; x++)
    {
        for (int y = 0; y < 6; y++)
        {
            for (int z = 0; z < 11; z++)
            {
                if (x+y+z < 10)
                {
                    Set_GridBlock(grid->grid, (Block){1}, x, y, z);
                }
            }
        }
    }

    ShipBP_Add_LogicBlock(shipBp, grid, 1, (short[3]){0, 0, 11}, 0);

    glm_mat3_copy(GLM_MAT3_IDENTITY, shipBp->rb.baseInertiaTensor);
    glm_mat3_scale(shipBp->rb.baseInertiaTensor, 10000);
    glm_mat3_inv(shipBp->rb.baseInertiaTensor, shipBp->rb.invBaseInertiaTensor);
    shipBp->rb.baseMass = 1000.0;
    //shipBp->rb.baseCenterOfMass = 

    Generate_MeshForGrid(grid);
    for (int i = 0; i < grid->matCount; i++)
    {
        res = res || DEBUG_VALID_MESH(grid->renderDatas[i]->mesh);
    }
    Calc_ShipBB(shipBp);
    if (shipBp->BB[0] < 8.3) {LOG_TEXT("!ShipBP test1 error: BB too small\n"); res = 1;}
    if (shipBp->BB[0] > 8.5) {LOG_TEXT("!ShipBP test1 error: BB too large\n"); res = 1;}
    *bp = shipBp;

    Save_ShipBlueprint(shipBp, "utest1_bp");

    return res;
}

int ShipBP2_Test(EngineState* engineState, ShipBP** bp)
{
    int res = 0;

    ShipBP* shipBp = (ShipBP*)calloc(1,sizeof(ShipBP));
    shipBp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState));
    Part* root = shipBp->model.rootPart;
    root->localTransform = (Transform){0.0,0.0,0.0,1.0,0.0,0.0,0.0,0};

    PartStructureGrid* grid = root->structure;
    grid->grid.array = (Block*)calloc(15*6*7, sizeof(Block));
    grid->grid.x_s = 15;
    grid->grid.y_s = 6;
    grid->grid.z_s = 7;
    grid->centerOffset[0] = 7.5;
    grid->centerOffset[1] = 3.0;
    grid->centerOffset[2] = 3.5;

    for (int x = 0; x < 15; x++)
    {
        for (int y = 0; y < 6; y++)
        {
            for (int z = 0; z < 7; z++)
            {
                if ((x+y+z) % 6 == 0)
                {
                    Set_GridBlock(grid->grid, (Block){1}, x, y, z);
                }
            }
        }
    }

    ShipBP_Add_LogicBlock(shipBp, grid, 1, (short[3]){0, 6, 11}, 0);

    glm_mat3_copy(GLM_MAT3_IDENTITY, shipBp->rb.baseInertiaTensor);
    glm_mat3_scale(shipBp->rb.baseInertiaTensor, 10000);
    glm_mat3_inv(shipBp->rb.baseInertiaTensor, shipBp->rb.invBaseInertiaTensor);
    shipBp->rb.baseMass = 1000.0;
    //shipBp->rb.baseCenterOfMass = 

    Generate_MeshForGrid(grid);
    for (int i = 0; i < grid->matCount; i++)
    {
        res = res || DEBUG_VALID_MESH(grid->renderDatas[i]->mesh);
    }
    Calc_ShipBB(shipBp);
    if (shipBp->BB[0] < 8.8) {LOG_TEXT("!ShipBP test1 error: BB too small\n"); res = 1;}
    if (shipBp->BB[0] > 8.9) {LOG_TEXT("!ShipBP test1 error: BB too large\n"); res = 1;}
    *bp = shipBp;

    Save_ShipBlueprint(shipBp, "utest2_bp");

    return res;
}

void Test_ShipBPs(EngineState* engineState)
{
    int res = 0;
    ShipBP* bp1 = NULL;
    ShipBP* bp2 = NULL;
    res += ShipBP1_Test(engineState, &bp1);
    res += ShipBP2_Test(engineState, &bp2);
    Delete_ShipBP(bp1);
    Delete_ShipBP(bp2);

    LOG_INFO("Failed %d ShipBP tests\n", res);
}

void Run_Tests(EngineState* engineState)
{
    Test_ShipBPs(engineState);
}