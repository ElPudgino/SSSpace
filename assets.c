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

void Load_LogicBlockDefs()
{
    blockDefs = (LogicBlockDef*)calloc(LOGIC_BLOCK_ID_COUNT, sizeof(LogicBlockDef));
    _Call_Init(BlockDef_Cannon_Block);
}

const LogicBlockDef* Get_LogicBlockDef(uint32_t index)
{
    assert(index < LOGIC_BLOCK_ID_COUNT);
    assert(index > 0);
    return &blockDefs[index];
}