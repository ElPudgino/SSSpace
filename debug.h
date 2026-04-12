#ifndef GAME_DEBUG
#define GAME_DEBUG

#include "mesh_utils.h"

#define DEBUG_VALID_MESH(mesh) DEBUG ? Debug_Validate_Mesh(mesh) : 0
#define DEBUG_PRINT_MESH(mesh) DEBUG ? Debug_Print_Mesh(mesh) : 0

int Debug_Validate_Mesh(Mesh* mesh);

void Debug_Print_Mesh(Mesh* mesh);

#endif