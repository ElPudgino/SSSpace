#include "sector.h"
#include "camera_control.h"
#include "physics.h"
#include "material_insts.h"
#include "logger.h"

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

// Upload sky stars data etc.
// will need more parameters to generate the data
void Load_SectorVisualData(EngineState* engineState, Sector* sector)
{
    if (SERVER) return;
    sector->visuals = (SectorVisualData*)calloc(1, sizeof(SectorVisualData));
    SkyStar st = {};

    // Temporary --
    sector->visuals->starCount = 5000;
    sector->visuals->stars = (SkyStar*)calloc(5000, sizeof(SkyStar));
    for (int i = 0; i < 5000; i++)
    {
        st.magn = (float)(rand() % 15) / 2000.0;
        st.type = 0;
        st.spos[0] = (float)((rand() % 90) - (rand() % 90)) * GLM_PI / 180.0;
        st.spos[1] = (float)((rand() % 360) -  180) * GLM_PI / 180.0;
        sector->visuals->stars[i] = st;
    }
    VkDeviceAddress addr = 0;
    BufferInfo info = {};
    if (Upload_Buffer(engineState, sector->visuals->stars, 5000 * sizeof(SkyStar), &addr, &info)) LOG_TEXT("!Failed to upload sector star data\n");

    sector->visuals->starBufferAddr = addr;
    sector->visuals->starBufferInfo = info;
    // --
}

void Unload_SectorVisualData(EngineState* engineState, Sector* sector)
{
    if (SERVER) return;
    assert(sector);
    assert(sector->visuals);

    if (sector->visuals->stars) free(sector->visuals->stars);
    if (sector->visuals->starBufferAddr) 
    {
        vmaDestroyBuffer(engineState->allocator,sector->visuals->starBufferInfo.buffer ,sector->visuals->starBufferInfo.allocation);
    }
    free(sector->visuals);

}

void Render_Sky(VkCommandBuffer bf, mat4 pv, Sector* sector)
{
    assert(!SERVER);
    assert(sector);
    assert(sector->visuals);
    assert(sector->visuals->starBufferAddr);

    Material* skymat = GetMaterial_Sky();

    mat4 m;
    glm_mat4_copy(pv, m);
    Material_SetParameter(skymat, 0, &m);
    Material_SetParameter(skymat, 1, &sector->visuals->starBufferAddr);

    Bind_Material(bf, skymat);
    
    vkCmdDraw(bf, 6, sector->visuals->starCount, 0, 0);

    return;
}

void Render_SectorObjects(EngineState* engineState, Sector* sector, mat4 pv)
{
    if (SERVER) return;
    double fwd[3];
    Get_CameraForwardD(fwd);

    for (int i = 0; i < sector->rawObjects_count; i++)
    {
        Render_Object(sector->rawObjects[i], pv, fwd);
    }

}

void Render_Sector(EngineState* engineState, Sector* sector, VkCommandBuffer cmnd)
{
    mat4 mat;
    Get_ProjViewMatrix(mat, engineState->frameData.drawImage.imageExtent);
    mat4 cpy;
    glm_mat4_copy(mat, cpy);
    Render_SectorObjects(engineState, sector, cpy);
    Render_Sky(cmnd, mat, sector);
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
    uint32_t temp = 0;
    for (int i = 0; i < SECTOR_HT_SIZE; i++)
    {
        temp = sector->hashtable[i];
        sector->hashtable[i] += (sector->hashtable[i] << 16) + curind;
        // high 16 bits: count
        // low 16 bits: endindex
        curind += temp;
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