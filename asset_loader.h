#ifndef ASSET_LOADER
#define ASSET_LOADER

#include "libs.h"

// TODO: Use a custom mesh format which holds data the same way as the game:
// List of vertices (pos,extra,norm,uv)
// List of indices
// Write a tool to convert objs to this format, 
// which would amount to merging vertices with the same pos and shuffling indices

typedef struct _subMesh
{
    uint32_t startInd;
    uint32_t len;
    char* matName;
} subMesh;

typedef struct _objData
{
    char* name;
    char* mtllib;
    float* pos;
    float* uvs;
    float* normals;
    uint32_t* faces;
    subMesh* submeshes;
    uint32_t pcount;
    uint32_t uvcount;
    uint32_t ncount;
    uint32_t facecount;
    uint32_t submeshCount;
} objData;

void Load_PartModelFromObj(EngineState* engineState, char* file, void** partStructure);

int Load_Models(EngineState* engineState);

int Unload_Models(EngineState* engineState);

#endif