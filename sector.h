#ifndef SECTOR
#define SECTOR

/*! 
* Contains static data and things which are accessible from the global map:
* e.g. celestial bodies in the sector
* also stuff like resources and POI
* When global map is loaded this data gets generated
*/
typedef struct _BaseSectorData
{

} BaseSectorData;


/*!
* Exists when the sector is active:
* On client only the current sector exists
* On server all which are loaded
* Sector data isnt generated until it becomes active
* Use an octree for spatial partitioning, use a pool for octree nodes
*/
typedef struct _Sector
{

} Sector;

#endif