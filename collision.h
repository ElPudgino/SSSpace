#ifndef COLLISION
#define COLLISION

#include "basic_objects.h"

typedef struct _BoundingBox 
{
    vec3 start;
    vec3 end;
} BoundingBox;

typedef struct _RaycastData
{
    float distance;
    vec3 pos;
    vec3 normal;
    uint32_t gridPos[3];
} RaycastData;

int Raycast_Box(BoundingBox bb, vec3 dir, vec3 spos, float* dist);

int Raycast_GridLocal(BlockGrid grid, vec3 s_dir, vec3 s_spos, RaycastData* data);

int Raycast_GridGlobal(Part* grid, vec3 s_dir, double s_spos[3], RaycastData* data);

#endif