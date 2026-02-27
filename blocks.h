#ifndef BLOCKS_COMMON
#define BLOCKS_COMMON

#include "mesh_gen.h"
#include "material_insts.h"

void _Init_Cube();

void Destroy_BlockModels();

BlockModel* Get_CubeModel(uint32_t sides);

#endif