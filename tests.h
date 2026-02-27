#ifndef SHIP_TESTS
#define SHIP_TESTS

#include "ship.h"
#include "mesh_gen.h"
#include "blocks.h"

Ship* Create_ShipFromBP(ShipBP* bp);

void create_testshipbp(EngineState* engineState);

ShipBP* get_testbp();

#endif