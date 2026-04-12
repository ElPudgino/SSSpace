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

// pointers to Ship, sector, etc.
typedef struct _UpdateData
{

} UpdateData;

typedef BlockModel* (*ModelGetter)(uint32_t visiblesides);
typedef void (*BlockRender)(void* instance, void* staticdata, mat4 prev);
typedef void (*OnUpdate)(UpdateData* data); // called every frame
/*!
Used for unique interations with the block
* The main way player's scripts can do things
* Arguments struct is filled by player's script
* @param msg A pointer to arguments struct 
* @attention Make sure incorrectly filled arguments dont cause issues.
* For example player may put NaN or Inf into a float argument
*/
typedef void (*OnMessage)(void* msg); 
typedef char* (*Serialize)(void* instance);
typedef int (*Deserialize)(void* instance, char* data);

/*!
* Defined with code
*/
typedef struct _LogicBlockDef
{
    ModelGetter modelGetter;
    BlockRender render;
    OnUpdate update;
    OnMessage message;
    uint32_t messageDataSize;
    uint32_t instanceDataSize;
    uint32_t ID;
    void* staticData;
} LogicBlockDef;

int Load_Models(EngineState* engineState);

const LogicBlockDef* Get_LogicBlockDef(uint32_t index);

int Destroy_LogicBlockDefs(EngineState* engineState);

int Load_LogicBlockDefs();

#endif