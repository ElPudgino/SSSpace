#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "htable.h"
#include "basic_objects.h"

#define MODEL_TABLE_SIZE 10000

HashTable modeltable;

typedef struct _modelelem{
	const char* str;
	Model* model;
} ModelElem;

size_t modelhashfunc(const void* v) {
	const ModelElem* cnt =(const ModelElem*)v;
	const char* str = cnt->str;
	size_t h = 0xf1310ef8e0fe7b01;
	while(*str) {
		h += (unsigned)(*str++) * 0x135a482151832101;
		h = (h << 41)  | (h >> 23);
	}
	return h;
}

void* modelelem_copy(const void* v) {
	ModelElem* c = (ModelElem*)malloc(sizeof(ModelElem));
	const ModelElem* orig = (const ModelElem*)v;
	*c = (ModelElem){strdup(orig->str), orig->model};
	return c;
}

int modelelem_equal(const void* a, const void* b) {
	const ModelElem* x = (const ModelElem*)a;
	const ModelElem* y = (const ModelElem*)b;
	return !strcmp(x->str, y->str);
}

// ! Elements are not removed from table, so its fine to Destroy_Model as it will only when table is freed
void modelelem_free(void* a) {
	ModelElem* x = (ModelElem*)a;
	Destroy_Model(x->model);
	free((void*)x->str);
	free(x);
}

Model* ModelTable_Get_Model(const char* name)
{
	ModelElem k = {name, 0};
	ModelElem* res;
	if (!ht_get(&modeltable, &k, (void**)&res)) return res->model;
	return NULL;
}

void ModelTable_Set_Model(const char* name, Model* model)
{
	ModelElem k = {name, model};
	if(ht_set(&modeltable, &k)) printf("!Error while adding model to modeltable\n");
}

int Init_ModelTable()
{
	if (ht_init(&modeltable, MODEL_TABLE_SIZE, modelhashfunc, modelelem_copy, modelelem_equal, modelelem_free)) 
		return printf("Cannot initialize\n"), 0;
	return 0;
}

void CleanUp_ModelTable()
{
	ht_free(&modeltable);
}