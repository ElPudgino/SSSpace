#include "collision.h"
#include "ship.h"

static const int expand = 10;
static ShipBP* BP = NULL;
static Part* part = NULL;

void Place_Block(Block b, int pos[3], BlockGrid* grid)
{
    assert(grid);
    assert(grid->array);
    // if out of bounds, resize grid and relocate blocks
    int x = 0, y = 0, z = 0;
    if (pos[0] < 0) x = -1;
    else if (pos[0] >= grid->x_s) x = 1;
    if (pos[1] < 0) y = -1;
    else if (pos[1] >= grid->y_s) y = 1;
    if (pos[2] < 0) z = -1;
    else if (pos[2] >= grid->z_s) z = 1;

    if (!x && !y && !z)
    {
        Set_GridBlock(*grid, b, x, y, z);
        return;
    }

    uint32_t sx = 0, sy = 0, sz = 0;
    sx = grid->x_s + x ? expand : 0;
    sy = grid->y_s + y ? expand : 0;
    sz = grid->z_s + z ? expand : 0;

    Block* newg = (Block*)calloc(sx*sy*sz, sizeof(Block));
    BlockGrid res = {.array = newg, .x_s = sx, .y_s = sy, .z_s = sz};
    Block bl = {};

    for (int pz = 0; pz < grid->z_s; pz++)
    {
        for (int py = 0; py < grid->y_s; py++)
        {
            for (int px = 0; px < grid->x_s; px++)
            {
                bl = Get_GridBlock(*grid, px, py, pz);
                Set_GridBlock(res, bl, x < 0 ? px + expand : px,
                        y < 0 ? py + expand : py,
                        z < 0 ? pz + expand : pz);
                
            }
        }
    }
    free(grid->array);
    *grid = res;

    pos[0] = x < 0 ? pos[0] + expand : pos[0];
    pos[1] = y < 0 ? pos[1] + expand : pos[1];
    pos[2] = z < 0 ? pos[2] + expand : pos[2];
    Place_Block(b, pos, grid);
}

void Start_Editor(ShipBP* bp, Part* part)
{
    
}
