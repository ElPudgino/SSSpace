#ifndef MESH_UTILS
#define MESH_UTILS

#include "engine_utils.h"
#include "engine_init.h"
#include "render_primitives.h"

Mesh* Create_Mesh(EngineState* engineState);

void Mesh_UploadData(Mesh* mesh);

void RenderMesh(VkCommandBuffer cmnd, Mesh* mesh, Material* mat);

void Destroy_Mesh(Mesh* mesh);

#endif