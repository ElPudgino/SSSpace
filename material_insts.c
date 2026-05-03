#include "materials/materials.h"
#include "material_insts.h"

static Material* test = NULL;
static Material* sky = NULL;
static Material* hull = NULL;

int Init_MaterialInstances(EngineState* engineState)
{
    test = Test_Instanced_Mat_Build(engineState);
    hull = Ship_Mat_Build(engineState);

    sky = Sky_Mat_Build(engineState);
    assert(test);
    assert(sky);
    assert(hull);
    return 0;
}

int Destroy_MaterialInstances(EngineState* engineState)
{
    Destroy_Material(test);
    Destroy_Material(sky);
    Destroy_Material(hull);
    return 1;
}

Material* GetMaterial_Test()
{
    return test;
}

Material* GetMaterial_Hull()
{
    return hull;
}

Material* GetMaterial_Sky()
{
    return sky;
}