#ifndef MODEL_TABLE
#define MODEL_TABLE

void* ModelTable_Get_Model(const char* name);

void ModelTable_Set_Model(const char* name, void* model);

int Init_ModelTable();

#endif