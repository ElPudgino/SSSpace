#ifndef VFX_PARAMS
#define VFX_PARAMS

#include "libs.h"

typedef struct _SkyStar
{
    vec2 spos;
    float magn;
    uint32_t type; // select a texture from atlas with this
} SkyStar;

#endif