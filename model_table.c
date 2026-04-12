#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "htable.h"
#include "basic_objects.h"

#define MODEL_TABLE_SIZE 10000

HashTable table;

typedef struct _elem{
	const char* str;
	void* model;
} Elem;

size_t hashfunc(const void* v) {
	const Elem* cnt =(const Elem*)v;
	const char* str = cnt->str;
	size_t h = 0xf1310ef8e0fe7b01;
	while(*str) {
		h += (unsigned)(*str++) * 0x135a482151832101;
		h = (h << 41)  | (h >> 23);
	}
	return h;
}

void* str_copy(const void* v) {
	Elem* c = (Elem*)malloc(sizeof(Elem));
	const Elem* orig = (const Elem*)v;
	*c = (Elem){strdup(orig->str), orig->model};
	return c;
}

int str_equal(const void* a, const void* b) {
	const Elem* x = (const Elem*)a;
	const Elem* y = (const Elem*)b;
	return !strcmp(x->str, y->str);
}

void str_free(void* a) {
	Elem* x = (Elem*)a;
	Destroy_Model(x);
	free((void*)x->str);
	free(x);
}

int print_counter(HashTable* ht, void* kv, void* data) {
	Elem* x = (Elem*)kv;
	printf("%p %s", x->model, x->str);
	return 0;
}

void* ModelTable_Get_Model(const char* name)
{
	Elem k = {name, 0};
	Elem* res;
	if (!ht_get(&table, &k, (void**)&res)) return res->model;
	return NULL;
}

void ModelTable_Set_Model(const char* name, void* model)
{
	Elem k = {name, model};
	if(ht_set(&table, &k)) printf("!Error while adding model to modeltable\n");
}

int Init_ModelTable()
{
	if (ht_init(&table, MODEL_TABLE_SIZE, hashfunc, str_copy, str_equal, str_free)) 
		return printf("Cannot initialize\n"), 0;
	return 0;
}

void CleanUp_ModelTable()
{
	ht_free(&table);
}