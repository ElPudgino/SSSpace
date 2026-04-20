#ifndef PART_TABLE
#define PART_TABLE

#include "basic_objects.h"

PartStructureGrid* PartTable_Get_Part(uint64_t Id);

void PartTable_Set_Part(PartStructureGrid* part);

void PartTable_RemovePartSingle(PartStructureGrid* part);

int Init_PartTable();

int CleanUp_PartTable(EngineState* engineState);

#endif