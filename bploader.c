#include "bploader.h"
#include "math_util.h"

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
    fprintf(f, "GRID: ID-%ld\n", grid->ID);
    fprintf(f, "GRIDSIZE: %d %d %d\n", grid->grid.x_s, grid->grid.y_s, grid->grid.z_s);
    Block b = {};
    //                id(num),u:(updir),f:(frontdir);
    uint32_t lsize = (8 + 1 + 7 + 1) * grid->grid.x_s + 32;
    char* line = (char*)calloc(lsize, sizeof(char));
    uint32_t ns = 0;
    char up = 0;
    char north = 0;

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
                ns += snprintf(line + ns, lsize-ns, ",u:%c,f:%c;", up, north);
            }
            fprintf(f, "%s\n", line);
            //memset(line, 0, lsize * sizeof(char));
        }
    }
    fprintf(f, "\n");
}

void _Write_Part(Part* p, FILE* f, uint32_t depth)
{   
    if (depth > 2000) return;

    fprintf(f, "%*sPART:\n", depth, pline);
    if (p->childrenCount > 0)
    {
        fprintf(f, "%*s{\n", depth, pline);
        for (int i = 0; i < p->childrenCount; i++)
        {
            _Write_Part(&p->children[i], f, depth+1);
        }
        fprintf(f, "%*s}\n", depth, pline);
    }
}

void Save_BP_ToSBP(ShipBP* bp, FILE* f)
{
    assert(bp);
    fprintf(f, "SHIP_BLUEPRINT: ID-%ld\n", bp->ID);

}
