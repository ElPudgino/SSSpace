#ifndef MODEL_TABLE
#define MODEL_TABLE

Model* ModelTable_Get_Model(const char* name);

void ModelTable_Set_Model(const char* name, Model* model);

int Init_ModelTable();

void CleanUp_ModelTable();

#endif