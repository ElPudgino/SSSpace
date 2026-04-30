#include "materials/materials.h"
#include "material_insts.h"

static Material* test = NULL;
static Material* sky = NULL;

int Init_MaterialInstances(EngineState* engineState)
{
    test = Test_Instanced_Mat_Build(engineState);
    sky = Sky_Mat_Build(engineState);
    assert(test);
    assert(sky);
    return 0;
}

int Destroy_MaterialInstances(EngineState* engineState)
{
    Destroy_Material(test);
    Destroy_Material(sky);
    return 1;
}

Material* GetMaterial_Test()
{
    return test;
}

Material* GetMaterial_Sky()
{
    return sky;
}