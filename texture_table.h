#ifndef TEXTURE_TABLE
#define TEXTURE_TABLE

#include "image_utils.h"

ImageData* TextureTable_Get_Texture(const char* name);

void TextureTable_Set_Texture(const char* name, ImageData* model);

int Init_TextureTable();

void CleanUp_TextureTable();

#endif