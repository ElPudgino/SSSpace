#ifndef ASSETS
#define ASSETS

#include "libs.h"
#include "basic_objects.h"
#include "model_table.h"

// What is needed:
// 1. Load models, materials, logic block defs from file
//    Assets are linked by name strings
//
// 
// 2. For logic blocks: form an array from block defs, so defs can be indexed by id (index) instead of name
//    When saving, store the mapping names->ids, so it doesnt change when reloading/changing assets
//
// 3?. For parts: make a hashtable from part models
//
// 



typedef BlockModel* (*ModelGetter)(uint32_t visiblesides);
typedef void (*BlockRender)(void* instance, void* staticdata, mat4 prev);

/*!
* Defined with code
*/
typedef struct _LogicBlockDef
{
    ModelGetter modelGetter;
    BlockRender render;
    uint32_t instanceDataSize;
    uint32_t ID;
    void* staticData;
} LogicBlockDef;

int Load_Models(EngineState* engineState);

const LogicBlockDef* Get_LogicBlockDef(uint32_t index);

#endif