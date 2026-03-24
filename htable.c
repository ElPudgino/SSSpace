#include <stdlib.h>
#include <stdio.h>
#include "htable.h"


#define NNULL(e) ((e)?(e)->next:NULL)

struct _hashentry {
	size_t h;
	void* kv;
	struct _hashentry* next;
};

int ht_init(HashTable* ht, size_t cap, hash_func hash,
							hash_copy copy, hash_equal eq,
							hash_free free) {
	HashEntry** table = calloc(cap, sizeof(HashEntry*));
	if ( !table ) return ht_error("Cannot allocate memory %s\n", "err");
	*ht = (HashTable){cap, 0, 0, hash, copy, eq, free, table};
	return 0;
}

static int _rehash(HashTable* ht) {
	HashEntry** nt = calloc(ht->cap*2, sizeof(HashEntry*));
	if ( !nt ) return ht_error("Cannot allocate new memory\n");
	size_t cap2 = ht->cap*2;
	size_t lmax = 0;
	ht_error("Doing rehash %lu->%lu", ht->cap, cap2);
	for( size_t h = 0; h < ht->cap; h++ ) {
		size_t l = 0;
		for(HashEntry* e = ht->table[h], *n = NNULL(e); e; e = n, n = NNULL(e), l++) {
			size_t nh  = e->h % cap2;
			e->next = nt[nh];
			nt[nh] = e;
		}
		lmax = l > lmax?l:lmax;
	}
	free(ht->table);
	ht->cap = cap2;
	ht->table = nt;
	ht->lmax = lmax;
	ht_error("Rehash done, lmax=%lu", lmax);
	return 0;
}

int ht_set(HashTable* ht, const void* kv) {
	size_t hv = ht->hash(kv);
	size_t h = hv % ht->cap;
	HashEntry** e;
	if( ht->size*10 > ht->cap*9) _rehash(ht);
	for(e = &ht->table[h]; *e; e = &(*e)->next) {
		if ((*e)->h == hv && ht->eq((*e)->kv, kv)) {
			ht->free((*e)->kv);
			(*e)->kv = ht->copy(kv);
			return 0;
		}
	}
	if ( !(*e = malloc(sizeof(HashEntry))))
		return ht_error("Cannot allocate element\n");
	ht->size++;
	return **e = (HashEntry){hv, ht->copy(kv), NULL}, 0;
}
/*
Student kv;
kv = (Student) { .id =1231};
Student *v;
ht_get(&kv, &v);
*/
int ht_get(HashTable* ht, const void* kv, void** v) {
	size_t hv = ht->hash(kv);
	size_t h = hv % ht->cap;
	HashEntry** e;
	for(e = &ht->table[h]; *e; e = &(*e)->next) {
		if ((*e)->h == hv && ht->eq((*e)->kv, kv))
			return *v = (*e)->kv, 0;
	}
	return -1;
}
int ht_del(HashTable* ht, const void* key) {
	size_t h = ht->hash(key) % ht->cap;
	HashEntry** e;
	for(e = &ht->table[h]; *e; e = &(*e)->next) {
		if( ht->eq((*e)->kv, key) ) {
			HashEntry *n = (*e)->next;
			ht->free((*e)->kv);
			free(*e);
			ht->size--;
			return *e = n, 0;
		}
	}
	return -1;
}

/** функция-коллбек посещения элемента */
typedef int (*hash_visit)(HashTable* ht, void* kv, void* data);

int ht_traverse(HashTable* ht, hash_visit hv, void* data) {
	for( size_t h = 0; h < ht->cap; h++ ) {
		for(HashEntry* e = ht->table[h]; e; e=e->next) 
			if (hv(ht, e->kv, data)) return 1;
	}
	return 0;
}

void ht_free(HashTable* ht) {
	for(size_t h = 0; h< ht->cap; h++ ) {
		for(HashEntry*e = ht->table[h], *n = NNULL(e); e; e=n, n=NNULL(e)) {
			ht->free(e->kv);
			free(e);
		}
	}
	free(ht->table);
}
