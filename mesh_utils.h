#ifndef MESH_UTILS
#define MESH_UTILS

#include "engine_utils.h"
#include "engine_init.h"
#include "render_primitives.h"

void Mesh_UploadData(Mesh* mesh);

void RenderMeshesInstanced(VkCommandBuffer cmnd, Mesh* mesh, Material* mat);

#endif