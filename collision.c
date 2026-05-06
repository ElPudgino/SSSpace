#include "collision.h"
#include "ship.h"

int Raycast_Box(BoundingBox bb, vec3 dir, vec3 spos, float* dist)
{
    glm_vec3_normalize(dir);
    vec3 start_ts = {};
    vec3 end_ts = {};
    float tmp = 0;

    float smax = -INFINITY;
    float emin = INFINITY;

    for (int i = 0; i < 3; i++)
    {
        if (dir[i] != 0) {start_ts[i] = (bb.start[i] - spos[i]) / dir[i]; end_ts[i] = ((bb.end[i] - spos[i]) / dir[i]);}
        else if (spos[i] > bb.end[i] || spos[i] < bb.start[i]) return 0;
        else {start_ts[i] = -INFINITY; end_ts[i] = INFINITY;}
        if (start_ts[i] > end_ts[i]) {tmp = start_ts[i]; start_ts[i] = end_ts[i]; end_ts[i] = tmp;}
        if (end_ts[i] < 0) return 0;
        smax = smax < start_ts[i] ? start_ts[i] : smax; 
        emin = emin > end_ts[i] ? end_ts[i] : emin;
    }    
    if (smax >= emin) return 0; // we need to enter the box on all axis before exiting it on any axis
    if (smax > 0) *dist = smax;
    else *dist = 0; // Starting inside bb

    return 1;
}

// Grid 0 is at 0; Doesnt use part center offset
int Raycast_GridLocal(BlockGrid grid, vec3 s_dir, vec3 s_spos, RaycastData* data)
{
    vec3 dir = {};
    vec3 spos = {};

    glm_vec3_copy(s_dir, dir);
    glm_vec3_copy(s_spos, spos);

    if (glm_vec3_norm(dir) == 0) return 0;
    glm_vec3_normalize(dir);

    vec3 dirn = {};
    glm_vec3_copy(dir, dirn);

    int mvec[3] = {}; // different vectors to avoid frequent conversions
    vec3 fmvec = {};
    for (int i = 0; i < 3; i++)
    {
        if (dir[i] > 0) {mvec[i] = 1; fmvec[i] = 1.0;}
        else if (dir[i] < 0) {mvec[i] = -1; fmvec[i] = 0.0;} 
    }

    // Check against grid bounding box
    BoundingBox gridbb = {0,0,0,(float)grid.x_s, (float)grid.y_s, (float)grid.z_s};
    float dist = 0;
    if (!Raycast_Box(gridbb, dir, spos, &dist)) return 0;
    if (dist > 0) // get to the edge of the grid bb
    {
        //dist -= 1e-2;
        glm_vec3_scale(dirn, dist, dir);
        glm_vec3_add(spos, dir, spos);
    }
    //printf("spos2: %f, %f, %f\n", spos[0],spos[1],spos[2]);

    vec3 fsg = {floorf(spos[0]), floorf(spos[1]), floorf(spos[2])};

    int sg[3] = {(int)fsg[0], (int)fsg[1], (int)fsg[2]};
    Block b = {};
    vec3 normal = {};
    vec3 ts = {};
    float mint = INFINITY;
    int minind = -1;
    

    while (1)
    {
        if (sg[0] < -1 || sg[1] < -1 || sg[2] < -1 || sg[0] > grid.x_s || sg[1] > grid.y_s || sg[2] > grid.z_s) return 0;
        b = Get_GridBlock(grid, sg[0], sg[1], sg[2]);
        if (b.blockType != 0) break;
        mint = INFINITY;
        for (int i = 0; i < 3; i++)
        {
            if (dirn[i] == 0) continue;
            ts[i] = (fmvec[i] + fsg[i] - spos[i]) / dirn[i];
            if (ts[i] < mint) {mint = ts[i]; minind = i;}
        }
        fsg[minind] += fmvec[minind] * 2 - 1;
        sg[minind] += mvec[minind];
        glm_vec3_scale(dirn, mint, dir);
        glm_vec3_add(dir, spos, spos);
        dist += mint;
        glm_vec3_zero(normal);
        normal[minind] = 1 - 2 * fmvec[minind];
    }

    data->distance = dist;
    data->gridPos[0] = sg[0];
    data->gridPos[1] = sg[1];
    data->gridPos[2] = sg[2];
    glm_vec3_copy(normal, data->normal);
    glm_vec3_copy(spos, data->pos);
    return 1;
}
