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
    struct _TransformArray* next;
} TransformArray;

void Add_TransformArray(Mesh* mesh);

void Destroy_TransformArrays();

void Add_MeshToRender(Mesh* mesh, mat4 trs);

#endif