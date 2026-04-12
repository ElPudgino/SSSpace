#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "htable.h"
#include "basic_objects.h"

#define TEXTURE_TABLE_SIZE 10000

HashTable texturetable;

typedef struct _texelem{
	const char* str;
	ImageData* tex;
} TexElem;

size_t texhashfunc(const void* v) {
	const TexElem* cnt =(const TexElem*)v;
	const char* str = cnt->str;
	size_t h = 0xf1310ef8e0fe7b01;
	while(*str) {
		h += (unsigned)(*str++) * 0x135a482151832101;
		h = (h << 41)  | (h >> 23);
	}
	return h;
}

void* texelem_copy(const void* v) {
	TexElem* c = (TexElem*)malloc(sizeof(TexElem));
	const TexElem* orig = (const TexElem*)v;
	*c = (TexElem){strdup(orig->str), orig->tex};
	return c;
}

int texelem_equal(const void* a, const void* b) {
	const TexElem* x = (const TexElem*)a;
	const TexElem* y = (const TexElem*)b;
	return !strcmp(x->str, y->str);
}


// ! Elements are not removed from table, so its fine to Destroy_Image as it will only when table is freed
void texelem_free(void* a) {
	TexElem* x = (TexElem*)a;
	vkDestroyImageView(x->tex->device, x->tex->imageView, NULL);
	vmaDestroyImage(x->tex->allocator, x->tex->image, x->tex->allocation);
	free((void*)x->str);
	free(x);
}

ImageData* TextureTable_Get_Texture(const char* name)
{
	TexElem k = {name, 0};
	TexElem* res;
	if (!ht_get(&texturetable, &k, (void**)&res)) return res->tex;
	return NULL;
}

void TextureTable_Set_Texture(const char* name, ImageData* img)
{
	TexElem k = {name, img};
	if(ht_set(&texturetable, &k)) printf("!Error while adding texture to texturetable\n");
}

int Init_TextureTable()
{
	if (ht_init(&texturetable, TEXTURE_TABLE_SIZE, texhashfunc, texelem_copy, texelem_equal, texelem_free)) 
		return printf("Cannot initialize\n"), 0;
	return 0;
}

void CleanUp_TextureTable()
{
	ht_free(&texturetable);
}