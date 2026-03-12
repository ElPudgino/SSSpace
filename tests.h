#ifndef SHIP_TESTS
#define SHIP_TESTS

#include "ship.h"
#include "mesh_gen.h"
#include "blocks.h"
#include "sector.h"

void create_testshipbp(EngineState* engineState);

void create_testsector();

ShipBP* get_testbp();

Sector* get_testsector();

#endif