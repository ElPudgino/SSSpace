#include "transform_utils.h"

uint32_t CurrentArrayIndex = 1;
TransformArray* GlobalTransformArrays = NULL;
uint32_t GlobalTransformCap = 0;

void _add_RenderTransform(TransformArray* array, mat4 matrix);

void Add_TransformArray(Mesh* mesh)
{
    assert(mesh);
    assert(mesh->ID == 0);
    mesh->ID = CurrentArrayIndex;

    if (!GlobalTransformArrays)
    {
        GlobalTransformArrays = (TransformArray*)calloc(4, sizeof(TransformArray));
        GlobalTransformCap = 4;
    }
    else if (GlobalTransformCap == CurrentArrayIndex - 1)
    {
        GlobalTransformArrays = (TransformArray*)realloc(GlobalTransformArrays, sizeof(TransformArray) * GlobalTransformCap * 2);
        GlobalTransformCap *= 2;
    }
    
    CurrentArrayIndex++;
}

void Destroy_TransformArrays()
{
    free(GlobalTransformArrays);
}

void Add_MeshToRender(Mesh* mesh, mat4 trs)
{
    assert(mesh);
    assert(mesh->ID > 0);
    _add_RenderTransform(&GlobalTransformArrays[mesh->ID-1], trs);
}

void _add_RenderTransform(TransformArray* array, mat4 matrix)
{
    assert(array);
    assert(array->array);
    if (array->cap == 0)
    {
        array->cap = 4;
        array->array = (mat4*)calloc(4, sizeof(mat4));
    }
    else if (array->cap == array->count)
    {
        uint32_t newsize = (uint32_t)((float)array->cap * TRM_ARRAY_RESIZE_COEF);
        array->array = (mat4*)realloc(array->array, sizeof(mat4) * newsize);
        array->cap = newsize;
    }
    glm_mat4_copy(matrix, array->array[array->count]);
    array->count++;
}