#ifndef BLOCKS_COMMON
#define BLOCKS_COMMON

#include "mesh_gen.h"
#include "material_insts.h"

int Init_Blocks();

int Destroy_BlockModels(EngineState* engineState);

BlockModel* Get_HullCubeModel(uint32_t sides);

BlockModel* Get_RockCubeModel(uint32_t sides);

#endif