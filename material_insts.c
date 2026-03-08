#include "materials/materials.h"
#include "material_insts.h"

Material* test;

int Init_MaterialInstances(EngineState* engineState)
{
    test = Test_Instanced_Mat_Build(engineState);
    assert(test);
    return 0;
}

int Destroy_MaterialInstances(EngineState* engineState)
{
    Destroy_Material(test);
    return 1;
}

Material* GetMaterial_Test()
{
    return test;
}