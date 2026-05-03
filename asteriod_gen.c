#include "asteroid_gen.h"
#include "logger.h"

void Fill_AsteroidPart(PartStructureGrid* grid, float size)
{
    uint32_t xs = (uint32_t)((float)(rand() % 5000 + 10000) / 10000.0 * size);
    uint32_t ys = (uint32_t)((float)(rand() % 5000 + 10000) / 10000.0 * size);
    uint32_t zs = (uint32_t)((float)(rand() % 5000 + 10000) / 10000.0 * size);
    grid->grid.x_s = xs;
    grid->grid.y_s = ys;
    grid->grid.z_s = zs;
    grid->grid.array = (Block*)calloc(xs * ys * zs, sizeof(Block));
    grid->centerOffset[0] = (float)(xs) / 2.0;
    grid->centerOffset[1] = (float)(ys) / 2.0;
    grid->centerOffset[2] = (float)(zs) / 2.0;
    float noise = 0;
    float dist = 0;
    vec3 b = {(float)xs / 2.0, (float)ys / 2.0, (float)zs / 2.0};
    vec3 p = {};
    vec3 tmp = {};

    for (int x = 0; x < xs; x++)
    {
        for (int y = 0; y < ys; y++)
        {
            for (int z = 0; z < zs; z++)
            {
                p[0] = (float)x - grid->centerOffset[0];
                p[1] = (float)y - grid->centerOffset[1];
                p[2] = (float)z - grid->centerOffset[2];
                glm_vec3_divs(p, sqrtf(size), tmp);
                noise = (glm_perlin_vec3(tmp) + 1.0) / 2.0;
                glm_vec3_div(p, b, tmp);
                dist = glm_vec3_norm(tmp);
                dist = 1.0 - dist;
                if (dist * (dist + noise) > 0.4)
                {
                    Set_GridBlock(grid->grid, (Block){2}, x, y, z);
                }
            }
        }
    }
}

ShipBP* Gen_AsteroidType(EngineState* engineState, uint32_t partcount, float size)
{
    LOG_INFO("Generating asteroid: partcount=%u, size=%f\n",partcount,size);
    assert(size < 100.0);
    assert(partcount < 20);

    ShipBP* bp = Create_ShipBP(0);
    bp->model.rootPart = Create_Part(Create_PartStructureGrid(engineState), NULL);
    Part* root = bp->model.rootPart;
    root->localTransform = (Transform){0.0,0.0,0.0,1.0,0.0,0.0,0.0,0};

    glm_mat3_copy(GLM_MAT3_IDENTITY, bp->rb.baseInertiaTensor);
    glm_mat3_scale(bp->rb.baseInertiaTensor, 10000);
    glm_mat3_inv(bp->rb.baseInertiaTensor, bp->rb.invBaseInertiaTensor);
    bp->rb.baseMass = 1000.0;

    PartStructureGrid* grid = root->structure;
    Fill_AsteroidPart(grid, size);
    Generate_MeshForGrid(grid);

    vec3 ofst = {};
    versor rt = {};
    double dofst[3] = {};

    if (partcount > 1)
    {
        root->children = (Part*)calloc(partcount - 1, sizeof(Part));
        root->childrenCount = partcount - 1;
        for (int i = 0; i < partcount - 1; i++)
        {
            Create_Part(Create_PartStructureGrid(engineState), root->children + i);

            ofst[0] = (float)(rand()%400-200);
            ofst[1] = (float)(rand()%400-200);
            ofst[2] = (float)(rand()%400-200);

            glm_euler_xyz_quat(ofst, rt);
            root->children[i].localTransform = (Transform){0.0,0.0,0.0,1.0,0.0,0.0,0.0,&root->localTransform};
            glm_vec4_copy(rt, root->children[i].localTransform.rotation);

            if (glm_vec3_norm(ofst) == 0) ofst[0] += 1.0;
            glm_normalize(ofst);
            glm_vec3_scale(ofst, size* 0.5, ofst);
            Cast_ToDouble(ofst, root->children[i].localTransform.pos);

            Fill_AsteroidPart(root->children[i].structure, size * 0.6);
            Generate_MeshForGrid(root->children[i].structure);
        }
    }

    Calc_ShipBB(bp);
    ShipBP_Init_LogicBlocks(bp);

    return bp;
}
