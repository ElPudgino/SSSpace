#ifndef MATERIAL_INSTANCES
#define MATERIAL_INSTANCES

int Init_MaterialInstances(EngineState* engineState);

int Destroy_MaterialInstances(EngineState* engineState);

Material* GetMaterial_Test();

Material* GetMaterial_Sky();

Material* GetMaterial_Hull();

#endif