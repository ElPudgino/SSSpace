#include "materials/materials.h"
#include "material_insts.h"

Material* test;

void Init_MaterialInstances(EngineState* engineState)
{
    test = Test_Instanced_Mat_Build(engineState);
}

void Destroy_MaterialInstances()
{
    Destroy_Material(test);
}

Material* GetMaterial_Test()
{
    return test;
}