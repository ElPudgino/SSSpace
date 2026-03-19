#include "ship.h"
#include "mesh_gen.h"
#include "sector.h"

void Set_PartTransform(Part* part, Transform* tr)
{
    part->localTransform = *tr;
    //self.mdoeltransform.matrixes[part.matrixindex] = ...
}

uint32_t Get_IndexFromPos(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z)
{
    return x + y * grid.x_s + z * grid.x_s * grid.y_s;
}

void Set_GridBlock(BlockGrid grid, Block block, uint32_t x, uint32_t y, uint32_t z)
{
    if (x < 0 || y < 0 || z < 0 || x >= grid.x_s || y >= grid.y_s || z >= grid.z_s) {/*printf("-grid position out of bounds\n");*/ return;}
    grid.array[Get_IndexFromPos(grid, x, y, z)] = block;
}

Block Get_GridBlock(BlockGrid grid, uint32_t x, uint32_t y, uint32_t z)
{
    if (x < 0 || y < 0 || z < 0 || x >= grid.x_s || y >= grid.y_s || z >= grid.z_s) {/*printf("-grid position out of bounds\n");*/ return (Block){0};}
    return grid.array[Get_IndexFromPos(grid, x, y, z)];
}

Part* Create_Part(void* structure)
{
    Part* res = (Part*)calloc(1, sizeof(Part));
    res->structure = structure;
    return res;
}

void Render_Part(Part* part, mat4 prev)
{
    assert(part);
    //printf("getting part structure\n");
    PartStructureSimpleMesh p = *(PartStructureSimpleMesh*)part->structure;
    //printf("got part structure\n");
    mat4 cur;
    Get_LocalRenderTransformMatrix(&part->localTransform, cur);
    glm_mat4_mul(prev, cur, cur);
    switch (p.structureType)
    {
        case PART_TYPE_GRID:
            Render_Grid((PartStructureGrid*)part->structure, cur);
            break;
        case PART_TYPE_SIMPLE_MESH:
            Render_SimpleMesh((PartStructureSimpleMesh*)part->structure, cur);
            break;
        case PART_TYPE_MULTI_MESH:
            Render_MultiMesh((PartStructureMultiMesh*)part->structure, cur);
            break;
        default:
            break;
    }
    for (int i = 0; i< part->childrenCount; i++)
    {
        Render_Part(&part->children[i], cur);
    }
    //printf("Finish render part\n");
}

void Render_Ship(Ship* ship, mat4 tr)
{
    assert(ship);
    assert(ship->model.rootPart);
    Render_Part(ship->model.rootPart, tr);
}

// TODO: Take into account rotation constraints on the parts to reduce bb size

void Get_PartStructureBBsize(PartStructureSimpleMesh* part, float bb[3])
{
    float a = 0;
    float b = 0;
    vec3 vec = {};
    PartStructureGrid* g = NULL;
    switch (part->structureType)
    {
        case PART_TYPE_GRID:
            g = (PartStructureGrid*)part;
            vec[0] = (float)g->grid.x_s - g->centerOffset[0];
            vec[1] = (float)g->grid.y_s - g->centerOffset[1];
            vec[2] = (float)g->grid.z_s - g->centerOffset[2];
            a = glm_vec3_norm(vec);
            b = glm_vec3_norm(g->centerOffset);
            a = a > b ? a : b;
            bb[0] = a;
            bb[1] = a;
            bb[2] = a;
            break;
        case PART_TYPE_SIMPLE_MESH:
            a = sqrtf((float)(part->bbsize[0]*part->bbsize[0] + part->bbsize[1]*part->bbsize[1] + part->bbsize[2]*part->bbsize[2]));

            bb[0] = a;
            bb[1] = a;
            bb[2] = a;
            break;
        
        default:
            break;
    }
}

void Get_PartBBsize(Part* p, float s[3])
{
    s[0] = 0;
    s[1] = 0;
    s[2] = 0;
    vec3 tm;
    vec3 tr;
    for (int i = 0; i < p->childrenCount; i++)
    {
        Get_PartBBsize(&p->children[i], tm);
        tr[0] = (float)p->children[i].localTransform.pos[0];
        tr[1] = (float)p->children[i].localTransform.pos[1];
        tr[2] = (float)p->children[i].localTransform.pos[2];
        glm_vec3_abs(tr, tr);
        glm_vec3_add(tm, tr, tm); 
        s[0] = tm[0] > s[0] ? tm[0] : s[0];
        s[1] = tm[1] > s[1] ? tm[1] : s[1]; 
        s[2] = tm[2] > s[2] ? tm[2] : s[2];  
    }
    Get_PartStructureBBsize((PartStructureSimpleMesh*)p->structure, tm);
    s[0] = tm[0] > s[0] ? tm[0] : s[0];
    s[1] = tm[1] > s[1] ? tm[1] : s[1]; 
    s[2] = tm[2] > s[2] ? tm[2] : s[2]; 
}

// BB vector is in local coords
void Calc_ShipBB(ShipBP* bp)
{
    Get_PartBBsize(bp->model.rootPart, bp->BB);
}

float Get_ShipBBsize(Ship* ship)
{
    return glm_vec3_norm(ship->BP->BB);
}

void _Create_ShipPart(Part* ship, Part* bp)
{
    Set_PartTransform(ship, &bp->localTransform);
    if (bp->childrenCount > 0) ship->children = (Part*)calloc(bp->childrenCount, sizeof(Part));
    ship->childrenCount = bp->childrenCount;
    ship->structure = bp->structure;
    for (int i = 0; i < bp->childrenCount; i++)
    {
        _Create_ShipPart(&ship->children[i],&bp->children[i]);
    }
}

Ship* Create_ShipFromBP(ShipBP* bp)
{
    Ship* res = (Ship*)calloc(1, sizeof(Ship));
    res->BP = bp;
    res->model.rootPart = (Part*)calloc(1, sizeof(Part));
    res->rb = bp->rb;

    _Create_ShipPart(res->model.rootPart, bp->model.rootPart);

    res->rb.root = &res->model.rootPart->localTransform;
    printf("Created ship from bp\n");
    return res;
}

void Delete_PartStructure(void* structure)
{
    uint32_t p = (*(PartStructureSimpleMesh*)structure).structureType;
    switch (p)
    {
        case PART_TYPE_GRID:
            Destroy_StructureGrid((PartStructureGrid*)structure);
            break;
        case PART_TYPE_SIMPLE_MESH:
            Destroy_TransformArray(((PartStructureSimpleMesh*)structure)->renderData);
            free(structure);
            break;
        case PART_TYPE_MULTI_MESH:
            for (int i = 0; i < ((PartStructureMultiMesh*)structure)->matCount; i++)
            {
                Destroy_TransformArray(((PartStructureMultiMesh*)structure)->renderDatas[i]);
            }
            free(((PartStructureMultiMesh*)structure)->renderDatas);
            free(structure);
            break;
        default:
            break;
    }
}

void _Delete_ShipPart(Part* part)
{
    for (int i = 0; i < part->childrenCount; i++)
    {
        _Delete_ShipPart(&part->children[i]);
    }
    free(part->children);
}

void Delete_Ship(Ship* ship)
{
    _Delete_ShipPart(ship->model.rootPart);

    free(ship->model.rootPart);
    free(ship);
}

void _Delete_ShipBPPart(Part* part)
{
    assert(part);
    for (int i = 0; i < part->childrenCount; i++)
    {
        _Delete_ShipPart(&part->children[i]);
    }
    // A ship may have mutiple parts of the same type
    // Cant delete them like this
    // Delete_PartStructure(part->structure);
    free(part->children);
}

void Delete_ShipBP(ShipBP* bp)
{
    _Delete_ShipBPPart(bp->model.rootPart);

    free(bp->model.rootPart);
    free(bp);
}
