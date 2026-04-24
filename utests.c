#include "sector.h"
#include "ship.h"
#include "mesh_gen.h"
#include "blocks.h"
#include "debug.h"
#include "logger.h"
#include "bploader.h"
#include "bploader.h"

int ShipBP1_Test(EngineState* engineState, ShipBP** bp)
{
    int res = 0;
    LOG_TEXT("Starting shipBP create test 1\n");

    ShipBP* shipBp = Create_ShipBP(0);
    shipBp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState), NULL);
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

    Grid_Add_LogicBlock(grid, 1, (short[3]){0, 0, 11}, 0);

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
    ShipBP_Init_LogicBlocks(shipBp);
    if (shipBp->BB[0] < 8.3) {LOG_TEXT("!ShipBP test1 error: BB too small\n"); res = 1;}
    if (shipBp->BB[0] > 8.5) {LOG_TEXT("!ShipBP test1 error: BB too large\n"); res = 1;}
    *bp = shipBp;

    Save_ShipBlueprint(shipBp, "utest1_bp");

    return res;
}

int ShipBP2_Test(EngineState* engineState, ShipBP** bp)
{
    int res = 0;
    LOG_TEXT("Starting shipBP create test 2\n");

    ShipBP* shipBp = Create_ShipBP(0);
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

    Grid_Add_LogicBlock(grid, 1, (short[3]){0, 6, 11}, 0);

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
    ShipBP_Init_LogicBlocks(shipBp);
    if (shipBp->BB[0] < 11.74) {LOG_TEXT("!ShipBP test1 error: BB too small\n"); res = 1;}
    if (shipBp->BB[0] > 11.75) {LOG_TEXT("!ShipBP test1 error: BB too large\n"); res = 1;}
    *bp = shipBp;
    

    Save_ShipBlueprint(shipBp, "utest2_bp");

    return res;
}

int ShipBP1_LoadTest(EngineState* engineState, ShipBP** bp)
{
    assert(bp);
    LOG_TEXT("Starting shipBP load test 1\n");
    ShipBP* b = Load_BP_FromSBP_File("loadtest_1", engineState);
    if (!b) return 1;
    *bp = b;
    return 0;
}

int Run_ShipTest(int (*test)(EngineState* es, ShipBP** bprint), EngineState* engineState)
{
    int res = 0;
    ShipBP* bp = NULL;
    res += test(engineState, &bp);
    if (!bp)
    {
        res++;
        LOG_TEXT("Failed test: shipBP created was NULL\n");
        return res;
    }
    Ship* ship = Create_ShipFromBP(bp);
    if (ship) Delete_Ship(ship);
    else
    {
        res++;
        LOG_TEXT("Failed test: ship created was NULL\n");
    }
    if (bp) Delete_ShipBP(bp);
    return res;
}

void Test_ShipBPs(EngineState* engineState)
{
    int res = 0;
    res += !!Run_ShipTest(ShipBP1_Test, engineState);
    res += !!Run_ShipTest(ShipBP2_Test, engineState);
    res += !!Run_ShipTest(ShipBP1_LoadTest, engineState);

    LOG_INFO("Failed %d ShipBP tests\n", res);
}

void Run_Tests(EngineState* engineState)
{
    Test_ShipBPs(engineState);
}