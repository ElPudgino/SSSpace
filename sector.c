#include "sector.h"
#include "camera_control.h"
#include "physics.h"

#define HASH_CONST_A 3547361
#define HASH_CONST_B 7165167
#define HASH_CONST_C 5673343

Sector* Init_Sector()
{
    Sector* s = (Sector*)calloc(1, sizeof(Sector));
    s->hashtable = (uint32_t*)calloc(SECTOR_HT_SIZE, sizeof(uint32_t));
    s->objects = (Ship**)calloc(SECTOR_OBJLIST_BASESIZE, sizeof(Ship*));
    s->objects_cap = SECTOR_OBJLIST_BASESIZE;
    s->rawObjects = (Ship**)calloc(SECTOR_OBJLIST_BASESIZE, sizeof(Ship*));
    s->rawObjects_cap = SECTOR_OBJLIST_BASESIZE;
    return s;
}

/*! Spatial partition notes:
* If object is moved during the frame, it is not updated until next frame
* Could be mitigated by using deltas during the frame instead of directly modifying position
*
* Important: objects can be in multiple chunks simultaneusly
*/

void _Get_ObjectBBsize(Ship* obj, double bb[3])
{
    assert(obj);

    float a = Get_ShipBBsize(obj);
    bb[0] = (double)a;
    bb[1] = (double)a;
    bb[2] = (double)a;

}

void _Get_ObjectPos(Ship* obj, double pos[3])
{
    assert(obj);
    Copy_dVec((obj)->model.rootPart->localTransform.pos, pos);

}

void Render_Object(Ship* obj, mat4 projview, double fwd[3])
{
    assert(obj);
    // TODO: frustum culling
    Render_Ship(obj, projview);
}

void Render_Sector(EngineState* engineState, Sector* sector)
{
    mat4 mat;
    mat4 proj;
    VkExtent3D _drawExtent = engineState->frameData.drawImage.imageExtent;
    Projection_Matrix(proj, (float)_drawExtent.height/(float)_drawExtent.width, 0.01f, 100.0f, GLM_PI * 0.3f);
    Get_CameraMatrix(mat);
    glm_mat4_mul(proj, mat, mat);
    double fwd[3];
    Get_CameraForwardD(fwd);

    for (int i = 0; i < sector->rawObjects_count; i++)
    {
        Render_Object(sector->rawObjects[i], mat, fwd);
    }

}

void Tick_Sector(Sector* sector)
{
    for (int i = 0; i < sector->rawObjects_count; i++)
    {
        Apply_Velocities(&sector->rawObjects[i]->rb, 0.003F);
    }
    Hash_ObjectArray(sector, sector->rawObjects, sector->rawObjects_count);
}

int32_t _PartPos(double val)
{
    return (int32_t)floor(val / SECTOR_PARTITION_SIZE);
}

uint32_t _HashFuncChunkInt(uint32_t x, uint32_t y, uint32_t z)
{
    int64_t h = x * HASH_CONST_A + y * HASH_CONST_B + z * HASH_CONST_C;
    return h & SECTOR_HT_MASK;
}

uint32_t _HashFunc(double pos[3])
{
    return _HashFuncChunkInt(_PartPos(pos[0]), _PartPos(pos[1]), _PartPos(pos[2]));
}

void Hash_ObjectArray(Sector* sector, Ship** objects, uint32_t objcount)
{
    assert(sector);
    assert(objects);

    // clear hashtable
    memset(sector->hashtable, 0, SECTOR_HT_SIZE * sizeof(uint32_t));

    // count objects in each partition
    int corners[6] = {}; // xmin, ymin, zmin, xmax, ymax, zmax
    double pos[3];
    double bb[3];
    uint32_t indx = 0;
    uint32_t indy = 0;
    uint32_t indz = 0;

    // Can probably be improved
    for (int i = 0; i < objcount; i++)
    {
        _Get_ObjectPos(objects[i], pos);
        _Get_ObjectBBsize(objects[i], bb);
        corners[0] = _PartPos(pos[0] - bb[0]); 
        corners[1] = _PartPos(pos[1] - bb[1]);
        corners[2] = _PartPos(pos[2] - bb[2]); 
        corners[3] = _PartPos(pos[0] + bb[0]);
        corners[4] = _PartPos(pos[1] + bb[1]);
        corners[5] = _PartPos(pos[2] + bb[2]);
        
        indx = _HashFuncChunkInt(corners[0], corners[1], corners[2]);
        for (int x = corners[0]; x < corners[3]; x++)
        {
            indy = indx;
            for (int y = corners[1]; y < corners[4]; y++)
            {
                indz = indy;
                for (int z = corners[2]; z < corners[5]; z++)
                {
                    sector->hashtable[indz]++;
                    indz = (indz + HASH_CONST_C) & SECTOR_HT_MASK;
                }
                indy = (indy + HASH_CONST_B) & SECTOR_HT_MASK;
            }
            indx = (indx + HASH_CONST_A) & SECTOR_HT_MASK;
        }
    }

    // calculate indexes in array for objects
    uint32_t curind = 0;
    for (int i = 0; i < SECTOR_HT_SIZE; i++)
    {
        sector->hashtable[i] += (sector->hashtable[i] << 16) + curind;
        // high 16 bits: count
        // low 16 bits: endindex
        curind += sector->hashtable[i];
    }

    // add objects to array
    for (int i = 0; i < objcount; i++)
    {
        _Get_ObjectPos(objects[i], pos);
        _Get_ObjectBBsize(objects[i], bb);
        corners[0] = _PartPos(pos[0] - bb[0]); 
        corners[1] = _PartPos(pos[1] - bb[1]);
        corners[2] = _PartPos(pos[2] - bb[2]); 
        corners[3] = _PartPos(pos[0] + bb[0]);
        corners[4] = _PartPos(pos[1] + bb[1]);
        corners[5] = _PartPos(pos[2] + bb[2]);
        
        indx = _HashFuncChunkInt(corners[0], corners[1], corners[2]);
        for (int x = corners[0]; x < corners[3]; x++)
        {
            indy = indx;
            for (int y = corners[1]; y < corners[4]; y++)
            {
                indz = indy;
                for (int z = corners[2]; z < corners[5]; z++)
                {
                    sector->hashtable[indz]--;
                    uint32_t ind = sector->hashtable[indz];
                    sector->objects[ind & 0x0000ffff] = objects[i];
                    indz = (indz + HASH_CONST_C) & SECTOR_HT_MASK;
                }
                indy = (indy + HASH_CONST_B) & SECTOR_HT_MASK;
            }
            indx = (indx + HASH_CONST_A) & SECTOR_HT_MASK;
        }
    }
}

void Get_ObjectArrInPartition(Sector* sector, double pos[3], uint16_t* ind, uint16_t* cnt)
{
    uint32_t r = sector->hashtable[_HashFunc(pos)];
    *ind = r & 0x0000ffff;
    *cnt = r >> 16;
}