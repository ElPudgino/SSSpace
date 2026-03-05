#ifndef TRANSFORM_UTILS
#define TRANSFORM_UTILS

#include "libs.h"
#include "render_primitives.h"
#include "math_util.h"
 
#define TRM_ARRAY_RESIZE_COEF 1.5f
#define GLOBAL_TRANSFORM_ARRAY_SIZE 640000 

typedef struct _Transform
{
    double pos[3];
    vec4 rotation; // quaternion
    struct _Transform* parent;
} Transform;

typedef struct _TransformArray
{
    mat4* array;
    uint32_t count;
    uint32_t cap;
    int valid;
    InstancedRenderData renderData; 
} TransformArray;

void Get_LocalRenderTransformMatrix(Transform* transform, mat4 dest);

void Get_TransformForward(Transform* tr, float dest[3]);

void Get_TransformUp(Transform* tr, float dest[3]);

void Get_TransformRight(Transform* tr, float dest[3]);

void Add_TransformArray(InstancedRenderData* mesh);

void Add_InstanceToRender(InstancedRenderData* mesh, mat4 trs);

InstancedRenderData* Create_InstanceData();

void Upload_Transforms(EngineState* engineState);

int Setup_TransformBuffer(EngineState* engineState, uint32_t size);

void Render_InstancedMeshes(EngineState* engineState, VkCommandBuffer cmnd);

int Destroy_TransformBuffer(EngineState* engineState);

void Destroy_TransformArray(InstancedRenderData* data);

#endif