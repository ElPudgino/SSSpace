#ifndef BLOCKS_COMMON
#define BLOCKS_COMMON

#include "mesh_gen.h"
#include "material_insts.h"

int Init_Blocks();

int Destroy_BlockModels(EngineState* engineState);

BlockModel* Get_CubeModel(uint32_t sides);

#endif