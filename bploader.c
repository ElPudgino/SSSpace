#include "bploader.h"
#include "math_util.h"
#include "logger.h"
#include <sys/stat.h>
#include <sys/types.h>

char pline[2048] = {};
BpLoader loader = {};

void Init_Loader()
{
    memset(pline, ' ', 2047);
    loader.saveLocation = "savedmodels";
}



ShipBP* Load_BP_FromSBP(FILE* f)
{
    fseek(f, 0, SEEK_END);
    uint32_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
}

void _Save_Grid(PartStructureGrid* grid, FILE* f)
{
    fprintf(f, "GRID: ID:%ld\n", grid->ID);
    fprintf(f, "GRIDSIZE: %d %d %d\n", grid->grid.x_s, grid->grid.y_s, grid->grid.z_s);
    fprintf(f, "LOGICBLOCKS:\n");

    char up = 0;
    char north = 0;
    LogicBlock lb = {};
    for (int i = 0; i < grid->logicBlockCount; i++)
    {
        lb = grid->logicBlocks[i];
        BlockRotation_ToDirName(lb.rot, &up, &north);
        fprintf(f, "ID:%d;POS:%d,%d,%d;U:%c,N:%c\n", lb.defIndex, lb.pos[0], lb.pos[1], lb.pos[2], up, north);
    }

    fprintf(f, "BLOCKS:\n");
    Block b = {};
    //                id(num),u:(updir),f:(frontdir);
    uint32_t lsize = (8 + 1 + 7 + 1) * grid->grid.x_s + 32;
    char* line = (char*)calloc(lsize, sizeof(char));
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
                if (!(b.blockType << 8)) {ns += snprintf(line + ns, lsize-ns, "-;"); continue;}
                BlockRotation_ToDirName(b.blockType >> 24, &up, &north);
                ns += snprintf(line + ns, lsize-ns, "%d", (b.blockType << 8) >> 8);
                ns += snprintf(line + ns, lsize-ns, ",U:%c,N:%c;", up, north);
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
    fprintf(f, "%.*sGRID ID: %ld\n", depth, pline, p->structure->ID);
    fprintf(f, "%.*sPOSITION: %f %f %f\n", depth, pline, p->localTransform.pos[0], p->localTransform.pos[1], p->localTransform.pos[2]);
    fprintf(f, "%.*sROTATION: %f %f %f %f\n", depth, pline, p->localTransform.rotation[0], p->localTransform.rotation[1], p->localTransform.rotation[2], p->localTransform.rotation[3]);
    fprintf(f, "%.*sCHILDREN\n", depth, pline);
    if (p->childrenCount > 0)
    {
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
    fprintf(f, "SHIP_BLUEPRINT: ID:%ld\n", bp->ID);
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