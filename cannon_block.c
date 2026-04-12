#include "assets.h"
#include "math_util.h"

typedef struct _static_data
{
    Model* base;
    Model* barrelBase;
    Model* barrel;
} static_data;

// May need a pool to store these datas
// Or maybe link them to the ship, and allocate an array of the exact size when ship is created
// So ship assembly counts the blocks, gets instance data length and allocs the array
typedef struct _instance_data
{
    float angleX; // barrel left/right
    float angleY; // barrel up/down
    float CurRecoil;
} instance_data; 

// instance is block instance data
// !! prev must not be changed
void _Render(void* instance, void* staticdata, mat4 prev)
{
    //printf("rendering b\n");
    static_data* sd = (static_data*)staticdata;
    instance_data* id = (instance_data*)instance;
    //printf("instdata: %f, %f, %f\n", id->angleX, id->angleY, id->CurRecoil);
    Render_Model(sd->base, prev);
    mat4 barrel;
    mat4 tr;
    float angles[3] = {id->angleX, id->angleY, 0};
    glm_euler_xyz(angles, barrel);
    Translation_Matrix_s(0, 2.0, 0, tr);
    glm_mat4_mul(tr, barrel, barrel);
    glm_mat4_mul(prev, barrel, barrel);
    Render_Model(sd->barrelBase, barrel);
    glm_mat4_mul(barrel ,(mat4){{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,-id->CurRecoil,1}}, barrel);
    Render_Model(sd->barrel, barrel);
}

LogicBlockDef BlockDef_Cannon_Block()
{
    LogicBlockDef def = {};
    static_data* data = (static_data*)calloc(1, sizeof(static_data));
    data->base = ModelTable_Get_Model("cannon_base.obj");
    data->barrel = ModelTable_Get_Model("cannon_barrel.obj");
    data->barrelBase = ModelTable_Get_Model("cannon_barrelbase.obj");
    assert(data->base);
    assert(data->barrel);
    assert(data->barrelBase);
    def.staticData = data;

    //Its mesh is not merged to the ship right now
    //Its possible to merge base to ship but still have barrel independent
    def.modelGetter = NULL; 

    def.render = _Render;
    def.ID = 1;
    def.instanceDataSize = sizeof(instance_data);
    return def;
}