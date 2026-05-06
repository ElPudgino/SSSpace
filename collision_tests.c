#include "logger.h"
#include "collision.h"

float _eps_eq(float a, float b) {return fabsf(a - b) < 1e-4;}

int Test_BoxRaycast(BoundingBox bb, vec3 dir, vec3 spos, float* dist, int trgres, float trgdist)
{
    float dst = 0;
    int res = Raycast_Box(bb, dir, spos, &dst);
    if (res != trgres) return 1;
    if (trgres == 0) return 0;
    if (!_eps_eq(dst, trgdist)) return 2;
    return 0;
}

int Test_GridRaycast(BlockGrid grid, vec3 dir, vec3 spos, int trgres, RaycastData trgdata)
{
    RaycastData data = {};
    int res = Raycast_GridLocal(grid, dir, spos, &data);
    if (res != trgres) return 1;
    if (trgres == 0) return 0;
    if (!_eps_eq(data.distance, trgdata.distance)) {printf("dist; trg=%f, res=%f\n", trgdata.distance, data.distance); return 2;}
    glm_vec3_sub(data.pos, trgdata.pos, data.pos);
    if (!_eps_eq(glm_vec3_norm(data.pos), 0)) return 3;
    glm_vec3_sub(data.normal, trgdata.normal, data.normal);
    if (!_eps_eq(glm_vec3_norm(data.normal), 0)) return 4;
    if (data.gridPos[0] != trgdata.gridPos[0] | 
        data.gridPos[1] != trgdata.gridPos[1] |
        data.gridPos[2] != trgdata.gridPos[2]) return 5;
    return 0;
}

void Log_Result(int res, int* counter, int testindex)
{
    if (res == 0) return;
    LOG_INFO("Test %d failed with code: %d\n", testindex, res);
    *counter += 1;
}

void _set_vector(vec3 vec, float x, float y, float z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

BlockGrid _get_test_grid()
{
    BlockGrid res = {};
    res.array = (Block*)calloc(15*5*8, sizeof(Block));
    res.x_s = 15;
    res.y_s = 5;
    res.z_s = 8;
    
    for (int x = 3; x < 10; x++) // a slab of blocks
    {
        for (int y = 0; y < 3;y++)
        {
            for (int z = 1; z < 5; z++)
            {
                Set_GridBlock(res, (Block){1}, x, y, z);
            }
        }
    }


    for (int y = 0; y < 5;y++) // checker board layer
    {
        for (int z = 0; z < 8; z++)
        {
            if ((y+z) % 2 == 0) Set_GridBlock(res, (Block){1}, 14, y, z);
        }
    }
    

    return res;
}

#define GRID_RAY_TEST {res = Test_GridRaycast(grid, dir, spos, trgres, data); Log_Result(res, &counter, testind++);}

void Run_GridRaycastTests()
{
    RaycastData data = {};
    int counter = 0;
    int testind = 1;
    int res = 0;
    vec3 dir = {};
    vec3 spos = {};
    int trgres = 0;
    BlockGrid grid = _get_test_grid();

    _set_vector(spos, 16, 0.5, 1.5);
    _set_vector(dir, -1, 0, 0);
    _set_vector(data.pos, 10, 0.5, 1.5);
    _set_vector(data.normal, 1, 0 ,0);
    data.distance = 6;
    data.gridPos[0] = 9;
    data.gridPos[1] = 0;
    data.gridPos[2] = 1;
    trgres = 1;
    GRID_RAY_TEST

    _set_vector(spos, 115, 101.5, 103.5);
    _set_vector(dir, -1, -1, -1);
    _set_vector(data.pos, 15, 1.5, 3.5);
    _set_vector(data.normal, 1, 0, 0);
    data.distance = sqrtf(30000);
    data.gridPos[0] = 14;
    data.gridPos[1] = 1;
    data.gridPos[2] = 3;
    trgres = 1;
    GRID_RAY_TEST

    _set_vector(spos, 114.6, 102, 103.5);
    _set_vector(dir, -1, -1, -1);
    _set_vector(data.pos, 14.6, 2, 3.5);
    _set_vector(data.normal, 0, 1, 0);
    data.distance = sqrtf(30000);
    data.gridPos[0] = 14;
    data.gridPos[1] = 1;
    data.gridPos[2] = 3;
    trgres = 1;
    GRID_RAY_TEST

    printf("Grid raycast tests: %d/%d\n", testind-1-counter, testind-1);

    free(grid.array);
}

#define BOX_RAY_TEST {res = Test_BoxRaycast(bb, dir, spos, &dist, trgres, trgdist); Log_Result(res, &counter, testind++);}

void Run_BoxRaycastTests()
{
    int counter = 0;
    int testind = 1;
    int res = 0;
    BoundingBox bb;
    vec3 st = {-2,1,5};
    vec3 e = {4,3,12};
    vec3 dir = {0,0,1};
    vec3 spos = {0,2,-30};
    int trgres = 1;
    float trgdist = 35;
    float dist = 0;
    glm_vec3_copy(st, bb.start);
    glm_vec3_copy(e, bb.end);
    BOX_RAY_TEST // 1

    _set_vector(dir, 0, 0, -1);
    _set_vector(spos, 1, 2, -30);
    _set_vector(bb.start, -2, 1, 5);
    _set_vector(bb.end, 4, 3, 12);
    trgres = 0;
    BOX_RAY_TEST // 2

    _set_vector(dir, 0, 0, 1);
    _set_vector(spos, 1, 0, -30);
    _set_vector(bb.start, -2, 1, 5);
    _set_vector(bb.end, 4, 3, 12);
    trgres = 0;
    BOX_RAY_TEST // 3

    _set_vector(dir, 1, 1, 1);
    _set_vector(spos, 1, -1, 1);
    _set_vector(bb.start, 0,0, 0);
    _set_vector(bb.end, 5, 6, 12);
    trgres = 1;
    trgdist = sqrtf(3.0);
    BOX_RAY_TEST // 4

    _set_vector(dir, 1, 1, 1);
    _set_vector(spos, -1, -1, -1);
    _set_vector(bb.start, 0,0, 0);
    _set_vector(bb.end, 5, 6, 12);
    trgres = 1;
    trgdist = sqrtf(3.0);
    BOX_RAY_TEST // 5

    _set_vector(dir, 1, 1, 1);
    _set_vector(spos, 0, -1, -1);
    _set_vector(bb.start, 0,0, 0);
    _set_vector(bb.end, 5, 6, 12);
    trgres = 1;
    trgdist = sqrtf(3.0);
    BOX_RAY_TEST // 6

    _set_vector(dir, 1, -1, 1);
    _set_vector(spos, 1, -1, 1);
    _set_vector(bb.start, 0,0, 0);
    _set_vector(bb.end, 5, 6, 12);
    trgres = 0;
    BOX_RAY_TEST // 7

    _set_vector(dir, -2, 5, -34);
    _set_vector(spos, 1, -0.5, 0.5);
    _set_vector(bb.start, -1, -1, -1);
    _set_vector(bb.end, 3, 1, 2);
    trgres = 1;
    trgdist = 0;
    BOX_RAY_TEST // 8

    printf("Box raycast tests: %d/%d\n", testind-1-counter, testind-1);
}

void Run_CollisionTests()
{
    Run_BoxRaycastTests();
    Run_GridRaycastTests();
}