#ifndef SAMPLERS
#define SAMPLERS

#include "libs.h"

int Create_Samplers(EngineState* engineState);

int Destroy_Samplers(EngineState* engineState);

VkSampler Get_PixelSampler();

#endif