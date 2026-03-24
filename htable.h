#ifndef _HASH_H_
#define _HASH_H_
#include <stddef.h>

/* 
 * HashTable ht;
 * ht_init(&ht, strdup, strcmp, free);
 * ...
 *
 * char* key;
 * int value;
 *
 * ht_set(&ht, key, value)
 *
 * ht_free(ht);
 */

typedef size_t (*hash_func)(const void*);
typedef void* (*hash_copy)(const void*);
typedef int (*hash_equal)(const void*, const void*);
typedef void (*hash_free)(void*);

typedef struct _hashentry HashEntry;

#define ht_error(str, ...) (fprintf(stderr, "\033[91mERROR: " str "\033[0m\n" __VA_OPT__(,) __VA_ARGS__), -1)

typedef struct _hashtable {
	size_t cap; /**< Размер таблицы */
	size_t size; /**< Суммарное количество элементов в таблице */
	size_t lmax; /**< Максимальный размер бакета */
	hash_func hash; /**< Хеш-функция */
	hash_copy copy; /**< Функция клонирования */
	hash_equal eq; /**< Функция сравнения */
	hash_free free; /**< Функция освобождения */
	HashEntry **table; /**< Массив элементов HashEntry* */
} HashTable;

int ht_init(HashTable* ht, size_t cap, hash_func hash,
							hash_copy copy, hash_equal eq,
							hash_free free);
int ht_set(HashTable* ht, const void* kv);
int ht_get(HashTable* ht, const void* key, void** v);
int ht_del(HashTable* ht, const void* key);

/** функция-коллбек посещения элемента */
typedef int (*hash_visit)(HashTable* ht, void* kv, void* data);

int ht_traverse(HashTable* ht, hash_visit hv, void* data); 

void ht_free(HashTable* ht);

#endif /* _HASH_H_ */
