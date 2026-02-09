#ifndef TRANSFORM_UTILS
#define TRANSFORM_UTILS

#include "libs.h"
#include "render_primitives.h"

#define TRM_ARRAY_RESIZE_COEF 1.5f

typedef struct _TransformArray
{
    mat4* array;
    uint32_t count;
    uint32_t cap;
    InstancedRenderData renderData; 
} TransformArray;

void Add_TransformArray(InstancedRenderData* mesh);

void Destroy_TransformArrays();

void Add_MeshToRender(InstancedRenderData* mesh, mat4 trs);

void Upload_Transforms(EngineState* engineState);

int Setup_TransformBuffer(EngineState* engineState, uint32_t size);

#endif