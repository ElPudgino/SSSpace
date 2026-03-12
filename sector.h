#ifndef SECTOR
#define SECTOR

#include "libs.h"
#include "math_util.h"
#include "ship.h"

#define SECTOR_HT_SIZE 2048
#define SECTOR_HT_MASK (SECTOR_HT_SIZE - 1)
#define SECTOR_OBJLIST_BASESIZE 2000
#define SECTOR_PARTITION_SIZE 1024


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
* Use an hashinng for spatial partitioning, use a pool for octree nodes
*/
typedef struct _Sector
{
    uint32_t* hashtable;
    Object** objects; // partitioned
    uint32_t objects_count;
    uint32_t objects_cap;
    Object** rawObjects; // full unsorted array
    uint32_t rawObjects_count;
    uint32_t rawObjects_cap;
} Sector;

Sector* Init_Sector();

void Render_Sector(EngineState* engineState, Sector* sector);

void Hash_ObjectArray(Sector* sector, Object** objects, uint32_t objcount);

void Get_ObjectArrInPartition(Sector* sector, vec3 pos, uint16_t* ind, uint16_t* cnt);

#endif