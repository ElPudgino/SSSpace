#include "assets.h"
#include "asset_loader.h"
#include "logic_blocks.h"

#define LOGIC_BLOCK_ID_COUNT 1000

LogicBlockDef* blockDefs;

void _Call_Init(LogicBlockDef (*init)())
{
    LogicBlockDef def = init();
    assert(def.ID > 0);
    assert(def.ID < LOGIC_BLOCK_ID_COUNT);
    assert(blockDefs[def.ID].ID == 0);
    blockDefs[def.ID] = def;
}

int Load_LogicBlockDefs()
{
    blockDefs = (LogicBlockDef*)calloc(LOGIC_BLOCK_ID_COUNT, sizeof(LogicBlockDef));
    if (!blockDefs) return 1;
    _Call_Init(BlockDef_Cannon_Block);
    return 0;
}

int Destroy_LogicBlockDefs(EngineState* engineState)
{
    for (int i = 0; i < LOGIC_BLOCK_ID_COUNT; i++)
    {
        if (blockDefs[i].ID && blockDefs[i].staticData)
        {
            free(blockDefs[i].staticData);
        }
    }
    free(blockDefs);
    return 1;
}

const LogicBlockDef* Get_LogicBlockDef(uint32_t index)
{
    assert(index < LOGIC_BLOCK_ID_COUNT);
    assert(index > 0);
    return &blockDefs[index];
}