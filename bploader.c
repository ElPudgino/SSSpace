#include "bploader.h"
#include "math_util.h"
#include "logger.h"
#include "part_table.h"
#include "mesh_gen.h"
#include <sys/stat.h>
#include <sys/types.h>

char pline[2048] = {};
BpLoader loader = {};

void Init_Loader()
{
    memset(pline, ' ', 2047);
    loader.saveLocation = "savedmodels";
}

int Load_GridLogicBlocks(FILE* f, PartStructureGrid* grid)
{
    uint32_t lstart = ftell(f);
    uint32_t mxlen = 10000;
    char buf[128];
    uint32_t count = 0;
    LogicBlock lb = {}; 
    char up = 0;
    char north = 0;
    while (mxlen--)
    {
        fgets(buf, 127, f);
        if (sscanf(buf, "%u;%hd,%hd,%hd;%c,%c", &lb.defIndex, lb.pos, lb.pos+1, lb.pos+2, &up, &north) == 6) count++;
        else break;
    }
    grid->logicBlockCap = count;
    grid->logicBlockCount = count;
    grid->logicBlocks = (LogicBlock*)calloc(count, sizeof(LogicBlock));

    fseek(f, lstart, SEEK_SET);
    for (int i = 0; i < count; i++)
    {
        fgets(buf, 127, f);
        sscanf(buf, "%u;%hd,%hd,%hd;%c,%c", &lb.defIndex, lb.pos, lb.pos+1, lb.pos+2, &up, &north);
        lb.rot = DirName_ToBlockRotation(up, north);
        grid->logicBlocks[i] = lb;
    }   
    return 0;
}

int Load_GridBlocks(FILE* f, PartStructureGrid* grid)
{
    uint32_t lsize = (8 + 1 + 7 + 1) * grid->grid.x_s + 32;
    char* line = (char*)calloc(lsize, sizeof(char));
    grid->grid.array = (Block*)calloc(grid->grid.x_s * grid->grid.y_s * grid->grid.z_s, sizeof(Block));
    Block* xarr = NULL;
    char up = 0;
    char north = 0;
    Block b = {};
    uint32_t posz = 0;
    uint32_t posy = 0;
    uint32_t posx = 0;
    for (int L = 0; L < grid->grid.y_s * grid->grid.z_s; L++)
    {
        fgets(line, lsize - 1, f);
        if (sscanf(line, "Z:%u,Y:%u", &posz, &posy) != 2) 
        {
            LOG_INFO("Sbp blocks loading error: encounted an invalid line (line %u in array)\n", L);
        }
        else 
        {
            xarr = grid->grid.array + grid->grid.x_s * (posy + posz * grid->grid.y_s);
            posx = 0;
            for (int i = 0; i < lsize && line[i] != '\n' && line[i] != '\0'; i++)
            {
                if (line[i] == ';')
                {
                    posx++;
                    if (sscanf(line + i, ";%u,%c,%c", &b.blockType, &up, &north) == 3)
                        b.blockType = (b.blockType << 8) >> 8 | ((uint32_t)DirName_ToBlockRotation(up,north) << 24);
                    else
                        b.blockType = 0;
                    xarr[posx-1] = b;
                }
            }
            if (posx < grid->grid.x_s) LOG_INFO("Sbp blocks loading error: Found %d entries out of %d expected\n", posx, grid->grid.x_s);
        }

    }
    return 0;
}

int Load_Grid_FromBP(FILE* f, PartStructureGrid* grid)
{
    assert(grid);
    char buf[128];
    uint32_t mxlen = 2000;
    uint32_t scannedsize = 0;
    uint32_t scannedlgblocks = 0;
    uint32_t scannedbloks = 0;
    uint32_t gsize[3];

    while (mxlen--)
    {
        fgets(buf, 127, f);
        if (buf[0] == ' ' || buf[0] == '\n') continue;
        if (buf[0] = '\0') break;
        if (!strcmp(buf, "GRIDEND\n")) break;
        if (sscanf(buf, "GRIDSIZE: %d %d %d", gsize, gsize+1, gsize+2) == 3) 
        {
            if (!scannedsize) 
            {
                scannedsize = 1; 
                grid->grid.x_s = gsize[0]; 
                grid->grid.y_s = gsize[1];
                grid->grid.z_s = gsize[2];
            }
            else {LOG_TEXT("Error parsing sbp grid: GRIDSIZE declared second time\n"); return 1;}
            continue;;
        }
        if (!strcmp(buf, "LOGICBLOCKS:\n"))
        {
            if (!scannedlgblocks) {scannedlgblocks = 1; Load_GridLogicBlocks(f, grid);}
            else {LOG_TEXT("Error parsing sbp grid: LOGICBLOCKS declared second time\n"); return 2;}
        }
        if (!strcmp(buf, "BLOCKS:\n"))
        {
            if (!scannedbloks && scannedsize) {scannedbloks = 1; Load_GridBlocks(f, grid);}
            else if (scannedsize){LOG_TEXT("Error parsing sbp grid: BLOCKS declared second time\n"); return 3;}
            else {LOG_TEXT("Error parsing sbp grid: BLOCKS declared before GRIDSIZE\n"); return 8;}
        }
        LOG_TEXT("Encountered an unexpected line GRID definition; stopping\n");
        return 4;
    }
    if (!scannedsize) {LOG_TEXT("Error parsing sbp grid: GRIDSIZE not declared\n"); return 5;}
    if (!scannedlgblocks) {LOG_TEXT("Error parsing sbp grid: LOGICBLOCKS not declared\n"); return 6;}
    if (!scannedbloks) {LOG_TEXT("Error parsing sbp grid: BLOCKS not declared\n"); return 7;}

    Generate_MeshForGrid(grid);

    return 0;
}

int Load_AllGrids_FromBP(FILE* f, EngineState* engineState)
{
    fseek(f, 0, SEEK_END);
    uint32_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint32_t mx = 10000;
    uint64_t id = 0;
    int res = 0;

    char line[256]; // TODO: properly handle cases with very long lines

    while (mx-- && fgets(line, 255, f))
    {
        if (sscanf(line, "GRID: ID: %lu", &id) == 1)
        {
            if (PartTable_Get_Part(id) == NULL)
            {
                PartStructureGrid* grid = Create_PartStructureGrid_ID(engineState, id);
                res += Load_Grid_FromBP(f, grid);
            }   
        }
    }   
    return res;
}

int Load_Part_FromSBP(FILE* f, Part* root)
{
    uint64_t id = 0;
    double pos[3] = {};
    vec4 rot = {};
    char line[128] = {};
    char nl = 0;
    
    fgets(line, 127, f);
    if (sscanf(line ," PART:%c", &nl) != 1 || nl != '\n') {LOG_TEXT("Failed to load part from sbp: invalid PART marker\n"); return 1;}

    fgets(line, 127, f);
    if (sscanf(line ," GRIDID: %lu", &id) != 1) {LOG_TEXT("Failed to load part from sbp: invalid GRIDID field\n"); return 1;}

    fgets(line, 127, f);
    if (sscanf(line ," POSITION: %lf %lf %lf", pos, pos+1, pos+2) != 3) {LOG_TEXT("Failed to load part from sbp: invalid POSITION field\n"); return 1;}

    fgets(line, 127, f);
    if (sscanf(line ," ROTATION: %f %f %f %f", rot, rot+1, rot+2, rot+3) != 4) {LOG_TEXT("Failed to load part from sbp: invalid ROTATION field\n"); return 1;}

    fpos_t p = {};
    fgetpos(f, &p);

    if (id == 0) {LOG_TEXT("Failed to load part from sbp: ID is 0\n"); return 1;}
    Create_Part(PartTable_Get_Part(id), root);
    if (!root->structure) {LOG_TEXT("Failed to load part from sbp: unable to find linked grid\n"); return 1;}

    Copy_dVec(pos, root->localTransform.pos);
    glm_vec4_copy(rot, root->localTransform.rotation);

    uint32_t ch = 0;

    fgets(line, 127, f);
    if (sscanf(line ," CHILDREN %u", &ch) == 1 && ch > 0) 
    {
        fgets(line, 127, f);
        if (sscanf(line ," {%c", &nl) != 1 || nl != '\n') {LOG_TEXT("Failed to load part from sbp: invalid CHILDREN field\n"); return 1;}  
        root->childrenCount = ch;
        root->children = (Part*)calloc(2, sizeof(Part));
        for (int i = 0; i < ch; i++)
        {
            Load_Part_FromSBP(f, root->children + i);
            root->children[i].localTransform.parent = &root->localTransform;
        }
        fgets(line, 127, f);
        if (sscanf(line ," }%c", &nl) != 1 || nl != '\n') {LOG_TEXT("Failed to load part from sbp: CHILDREN array not closed\n"); return 1;} 
    }
    else fsetpos(f, &p);
    
    return 0;
}

ShipBP* Load_BP_FromSBP(FILE* f, EngineState* engineState)
{
    if (Load_AllGrids_FromBP(f, engineState)) LOG_TEXT("!Failed to load sbp\n");

    fseek(f, 0, SEEK_SET);
    uint64_t id = 0;
    char line[128] = {};
    fgets(line, 127, f);
    if (sscanf(line, "SHIP_BLUEPRINT: ID: %lu", &id) != 1) 
    {
        LOG_TEXT("Failed to load sbp: SHIP_BLUEPRINT line missing or invalid\n");
        return NULL;
    }

    ShipBP* res = Create_ShipBP(id);
    if (Load_Part_FromSBP(f, res->model.rootPart))
    {
        Delete_ShipBP(res);
        return NULL;
    }

    // TODO: Replace this placeholder with actual calculation
    glm_mat3_copy(GLM_MAT3_IDENTITY, res->rb.baseInertiaTensor);
    glm_mat3_scale(res->rb.baseInertiaTensor, 10000);
    glm_mat3_inv(res->rb.baseInertiaTensor, res->rb.invBaseInertiaTensor);
    res->rb.baseMass = 1000.0;

    Calc_ShipBB(res);
    ShipBP_Init_LogicBlocks(res);
    LOG_TEXT("Successfuly loaded a ShipBP from sbp file\n");
    return res;
}

void _Save_Grid(PartStructureGrid* grid, FILE* f)
{
    fprintf(f, "GRID: ID: %ld\n", grid->ID);
    fprintf(f, "GRIDSIZE: %d %d %d\n", grid->grid.x_s, grid->grid.y_s, grid->grid.z_s);
    fprintf(f, "LOGICBLOCKS:\n");

    char up = 0;
    char north = 0;
    LogicBlock lb = {};
    for (int i = 0; i < grid->logicBlockCount; i++)
    {
        lb = grid->logicBlocks[i];
        BlockRotation_ToDirName(lb.rot, &up, &north);
        fprintf(f, "%u;%hd,%hd,%hd;%c,%c\n", lb.defIndex, lb.pos[0], lb.pos[1], lb.pos[2], up, north);
    }

    fprintf(f, "BLOCKS:\n");
    Block b = {};
    //                id(num),u:(updir),f:(frontdir);
    uint32_t lsize = (8 + 1 + 7 + 1) * grid->grid.x_s + 32;
    char* line = (char*)calloc(lsize, sizeof(char));
    //char line[lsize];
    uint32_t ns = 0;
    

    for (int z = 0; z < grid->grid.z_s; z++)
    {
        for (int y = 0; y < grid->grid.y_s; y++)
        {
            ns = 0;
            ns += snprintf(line, lsize, "Z:%d,Y:%d ", z, y);
            for (int x = 0; x < grid->grid.x_s; x++)
            {
                b = grid->grid.array[Get_IndexFromPos(grid->grid, x, y, z)];
                if (!(b.blockType << 8)) {ns += snprintf(line + ns, lsize-ns, ";-"); continue;}
                BlockRotation_ToDirName(b.blockType >> 24, &up, &north);
                ns += snprintf(line + ns, lsize-ns, ";%d", (b.blockType << 8) >> 8);
                ns += snprintf(line + ns, lsize-ns, ",%c,%c", up, north); // rotation is set by directions of Up,North
            }
            fprintf(f, "%s\n", line);
            //memset(line, 0, lsize * sizeof(char));
        }
    }
    fprintf(f, "GRIDEND\n\n");
}

void _Write_Part(Part* p, FILE* f, uint32_t depth)
{   
    if (depth > 2000) return;
    fprintf(f, "%.*sPART:\n", depth, pline);
    fprintf(f, "%.*sGRIDID: %ld\n", depth, pline, p->structure->ID);
    fprintf(f, "%.*sPOSITION: %f %f %f\n", depth, pline, p->localTransform.pos[0], p->localTransform.pos[1], p->localTransform.pos[2]);
    fprintf(f, "%.*sROTATION: %f %f %f %f\n", depth, pline, p->localTransform.rotation[0], p->localTransform.rotation[1], p->localTransform.rotation[2], p->localTransform.rotation[3]);
    if (p->childrenCount > 0)
    {
        fprintf(f, "%.*sCHILDREN %u\n", depth, pline, p->childrenCount);
        fprintf(f, "%.*s{\n", depth, pline);
        for (int i = 0; i < p->childrenCount; i++)
        {
            _Write_Part(&p->children[i], f, depth+1);
        }
        fprintf(f, "%.*s}\n", depth, pline);
    }
}

void _Save_PartGrids(Part* p, FILE* f)
{
    _Save_Grid(p->structure, f);
    for (int i = 0; i < p->childrenCount; i++)
    {
        _Save_PartGrids(&p->children[i], f);
    }
}

void _Save_BP_ToSBP(ShipBP* bp, FILE* f)
{
    assert(bp);
    fprintf(f, "SHIP_BLUEPRINT: ID: %ld\n", bp->ID);
    _Write_Part(bp->model.rootPart, f, 0);
    fprintf(f, "\n");
    _Save_PartGrids(bp->model.rootPart, f);
}

char fname[256] = {};

void Save_ShipBlueprint(ShipBP* bp, char* filename)
{
    Init_Loader();
    mkdir("savedmodels", 0777);
    snprintf(fname, 255, "%s/%s.sbp", loader.saveLocation, filename);
    FILE* f = fopen(fname, "w");
    if (!f) {LOG_INFO("!Failed to create/open file: %s\n", fname); return;}
    _Save_BP_ToSBP(bp, f);
    fclose(f);
}