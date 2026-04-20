#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "htable.h"
#include "basic_objects.h"
#include "logger.h"

#define PART_TABLE_SIZE 10000

HashTable parttable;

typedef struct _partelem{
	PartStructureGrid* part;
} PartElem;

size_t parthashfunc(const void* v) {
	const PartElem* cnt =(const PartElem*)v;
	size_t h = 0xf1310ef8e0fe7b01;
	for (int i = 0; i < 64; i++) {
		h += (unsigned)(cnt->part->ID >> i) * 0x135a482151832101;
		h = (h << 41)  | (h >> 23);
	}
	return h;
}

void* partelem_copy(const void* v) {
	PartElem* c = (PartElem*)malloc(sizeof(PartElem));
	const PartElem* orig = (const PartElem*)v;
	*c = (PartElem){orig->part};
	return c;
}

int partelem_equal(const void* a, const void* b) {
	const PartElem* x = (const PartElem*)a;
	const PartElem* y = (const PartElem*)b;
	return x->part->ID == y->part->ID;
}

// ! Elements are not removed from table, so its fine to Destroy_Model as it will only when table is freed
void partelem_free(void* a) {
	PartElem* x = (PartElem*)a;
    if (x->part->userCount > 0) LOG_INFO("!Part with ID:%ld had %d usercount when it was destroyed\n", x->part->ID, x->part->userCount);
	Destroy_StructureGrid(x->part);
	free(x);
}

PartStructureGrid* PartTable_Get_Part(uint64_t Id)
{
    PartStructureGrid p;
    p.ID = Id;
	PartElem k = {&p};
	PartElem* res;
	if (!ht_get(&parttable, &k, (void**)&res)) return res->part;
	return NULL;
}

void PartTable_Set_Part(PartStructureGrid* part)
{
	PartElem k = {part};
    PartElem* res;
    if (!ht_get(&parttable, &k, (void**)&res)) 
    {
        res->part->userCount++;
    }
	else 
	{
		k.part->userCount = 0;
		if(ht_set(&parttable, &k)) printf("!Error while adding part to parttable\n");
	}
}

void PartTable_RemovePartSingle(PartStructureGrid* part)
{
    PartElem k = {part};
    PartElem* res;
    if (!ht_get(&parttable, &k, (void**)&res)) 
    {
        if (res->part->userCount > 0) res->part->userCount--;
        else LOG_TEXT("!Tried to decrease usercount on a part with no users\n");
    }
    else
    {
        LOG_TEXT("!Tried to decrease usercount on a part not in the table\n");
    }
}

int Init_PartTable()
{
	if (ht_init(&parttable, PART_TABLE_SIZE, parthashfunc, partelem_copy, partelem_equal, partelem_free)) 
		return printf("Cannot initialize\n"), 0;
	return 0;
}

int CleanUp_PartTable(EngineState* engineState)
{
	ht_free(&parttable);
}