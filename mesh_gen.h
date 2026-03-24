#ifndef MESH_GEN
#define MESH_GEN

#include "ship.h"

/*! @brief Uses blocks from grid to generate its mesh
* Should not be called twice on the same grid without destroying its InstancedRenderDatas
* Uploads newly generated meshes
* @param grid PartStructureGrid for which to generate mesh
*/
void Generate_MeshForGrid(PartStructureGrid* grid);

/*! @brief Returns sides which are neighbouring with other blocks 
* Neighbours out of bounds are considered empty
* @param BlockGrid Grid in which to check
* @param x X coordinate
* @param y Y coordinate
* @param z Z coordinate
*/
BlockSide Get_Adjacent(BlockGrid g, uint32_t x, uint32_t y, uint32_t z);

/*! @brief Adds block mesh to the main grid mesh
* Supports adding blocks with different materials
* @param grid Grid to which the block model will be added 
* @param bmodel Block model: mesh and material
* @param posx X coord of the target position in the grid
* @param posy Y coord of the target position in the grid
* @param posz Z coord of the target position in the grid
* @param rot How block mesh will be oriented
*/
void Add_ModelToPart(PartStructureGrid* grid, BlockModel* bmodel,uint32_t posx,uint32_t posy,uint32_t posz,BlockRotation rot);



#endif