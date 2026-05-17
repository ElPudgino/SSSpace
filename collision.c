#include "collision.h"

#define OBB_SIZE_MULT 0.8

void Get_Part_OBB_LocalPos(Part* part, OBB* obb)
{
    assert(part);
    assert(obb);
    obb->center[0] = ((float)part->structure->grid.x_s) / 2.0- part->structure->centerOffset[0];
    obb->center[1] = ((float)part->structure->grid.y_s) / 2.0- part->structure->centerOffset[1];
    obb->center[2] = ((float)part->structure->grid.z_s) / 2.0- part->structure->centerOffset[2];

    obb->sizes[0] = (float)part->structure->grid.x_s / 2.0 * OBB_SIZE_MULT;
    obb->sizes[1] = (float)part->structure->grid.y_s / 2.0 * OBB_SIZE_MULT;
    obb->sizes[2] = (float)part->structure->grid.z_s / 2.0 * OBB_SIZE_MULT;

    vec3 ax = {1,0,0};
    versor q = {};
    Get_GlobalRotation(&part->localTransform, q);
    glm_quat_rotatev(q, ax, ax);
    glm_vec3_copy(ax, obb->ortAxis[0]);
    ax[0] = 0;
    ax[1] = 1;
    ax[2] = 0;
    glm_quat_rotatev(q, ax, ax);
    glm_vec3_copy(ax, obb->ortAxis[1]);
    ax[0] = 0;
    ax[1] = 0;
    ax[2] = 1;
    glm_quat_rotatev(q, ax, ax);
    glm_vec3_copy(ax, obb->ortAxis[2]);
}

int Check_OBBs_AxisSep(OBB* obb1, OBB* obb2, vec3 ax)
{
    float dst = fabsf(glm_vec3_dot(obb1->center, ax) - glm_vec3_dot(obb2->center, ax));

    float ra = 0.0f, rb = 0.0f;
    for (int i = 0; i < 3; i++) 
    {
        ra += obb1->sizes[i] * fabsf(glm_vec3_dot(obb1->ortAxis[i], ax));
        rb += obb2->sizes[i] * fabsf(glm_vec3_dot(obb2->ortAxis[i], ax));
    }

    return dst > ra + rb + 1e-6f;  
}

int Check_OBB_Intersect(OBB* a, OBB* b)
{
    vec3 ax = {};

    for (int i = 0; i < 3; i++) 
    {
        if (Check_OBBs_AxisSep(a, b, a->ortAxis[i]))
            return 0;
    }

    for (int i = 0; i < 3; i++) 
    {
        if (Check_OBBs_AxisSep(a, b, b->ortAxis[i]))
            return 0;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm_vec3_cross(a->ortAxis[i], b->ortAxis[j], ax);
            if (glm_vec3_norm(ax) < 1e-6f) continue; 
            glm_vec3_normalize(ax);
            if (Check_OBBs_AxisSep(a, b, ax))
                return 0;
        }
    }

    return 1;
}

int Check_Part_OBB_Intersect(Part* p1, Part* p2)
{
    OBB a = {};
    OBB b = {};
    vec3 tr = {};
    double ps1[3] = {};
    double ps2[3] = {};
    Get_GlobalPosition(&p1->localTransform, ps1);
    Get_GlobalPosition(&p2->localTransform, ps2);
    tr[0] = (float)(ps1[0]-ps2[0]);
    tr[1] = (float)(ps1[1]-ps2[1]);
    tr[2] = (float)(ps1[2]-ps2[2]);

    // Absolute positions can be very large, but relative positions can not (objects are in the same chunk)
    Get_Part_OBB_LocalPos(p1, &a);
    glm_vec3_add(a.center, tr, a.center); // Put first obb at origin
    Get_Part_OBB_LocalPos(p2, &b);
    return Check_OBB_Intersect(&a, &b);
}

int _check_ship_part_intersect_rec(Part* a, Part* b)
{
    if (Check_Part_OBB_Intersect(a, b)) return 1;

    for (int i = 0; i < a->childrenCount; i++)
    {
        if (_check_ship_part_intersect_rec(&a->children[i], b)) return 1;
    }

    for (int i = 0; i < b->childrenCount; i++)
    {
        if (_check_ship_part_intersect_rec(&b->children[i], a)) return 1;
    }
    return 0;
}

int Check_Ship_POBB_Intersect(Ship* s1, Ship* s2)
{
    return _check_ship_part_intersect_rec(s1->model.rootPart, s2->model.rootPart);
}

int Raycast_Box(BoundingBox bb, vec3 dir, vec3 spos, float* dist)
{
    assert(dist);
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

int Raycast_GridGlobal(Part* grid, vec3 s_dir, double s_spos[3], RaycastData* data)
{
    vec3 dir = {};
    double spos[3] = {};
    versor q = {};
    Get_GlobalPosition(&grid->localTransform, spos);
    Get_GlobalRotation(&grid->localTransform, q);
    glm_quat_inv(q, q);
    Scalar_Mult(spos, -1);
    Add_dVec(spos, s_spos); // Align grid center with 0,0,0
    double coffset[3] = {};
    Cast_ToDouble(grid->structure->centerOffset, coffset);
    Scalar_Mult(coffset, -1);
    Add_dVec(coffset, spos); // Align grid 0,0,0 with 0,0,0
    glm_quat_rotatev(q, s_dir, dir);
    vec3 pos = {(float)spos[0], (float)spos[1], (float)spos[2]};
    return Raycast_GridLocal(grid->structure->grid, dir, pos, data);   
}