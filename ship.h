#ifndef GAME_SHIP
#define GAME_SHIP

#include "libs.h"

typedef struct _Part
{
    mat4 localTransform;
    uint32_t matrixIndex;
    _Part* children;
} Part;

typedef struct _Model
{
    Part* rootPart;
    mat4* globalTransformArray;
} Model;

// map all parts to an array of matrixes (mapping is persistent for each model)
// when rendering model apply the mapped matrix to the part
// rendering:
// instanced,
// 1 option 
// parts global transforms are stored in 1 array for each unique part
// before rendering for each part write final transform matrix at the end of correspondong array
// every frame this array starts at 0
// another option:
// form a large array of modelTransforms,
// if we have n instances of a model, k matrixes per model
// render mesh1 n times with offset k
// send parent tree indexes to gpu, apply local transforms there
// parent tree is per model, not per instance 

// try 1 option first
// if chunk in which mesh is is invisible just dont add its matrix to the array
// special effects follow the same principle
// just add an array for each special effect
// specific renderers can just add a matrix to one of the arrays to render something
// adding instance data to the array can be done anytime in the frame (need to wait for copy-to-gpu fence though)

typedef struct _ModelTransform
{
    mat4* matrixes;
} ModelTransform;

typedef struct _ShipSharedData
{
    Model* model;
} ShipSharedData;

typedef struct _Ship
{
    ModelTransform modelTransform;
    ShipSharedData shared;
} Ship;

#endif