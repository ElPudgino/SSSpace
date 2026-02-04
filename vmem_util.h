#ifndef VMEM_UTILS
#define VMEM_UTILS

#include "engine_utils.h"
#include "engine_init.h"
#include "render_primitives.h"

void Mesh_UploadData(Mesh* mesh);

BufferInfo CreateBuffer(VmaAllocator alloc, size_t size, VmaMemoryUsage mem_property, VkBufferUsageFlags buf_usage);

void RenderMesh(VkCommandBuffer cmnd, Mesh* mesh, Material* mat, mat4 TRS);

#endif