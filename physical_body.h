#ifndef PHYSICAL_BODY
#define PHYSICAL_BODY

#include "math_util.h"
#include "basic_objects.h" 

typedef void (*TensorGetter)(Object* owner, mat3 dest);
typedef double (*ValGetter)(Object* owner);
typedef void (*DVectorGetter)(Object* owner, double dest[3]);
typedef void (*VectorGetter)(Object* owner, vec3 dest);

typedef struct _RigidBody
{
    Object* owner;
    TensorGetter inertiaTensor;
    ValGetter mass;
    ValGetter volume;
    VectorGetter centerOfMass;

    vec3 velocity;
    vec3 angularVelocity;
     
    // ! Center of mass can change depending on cargo and other stuff like docked ships
    // 
} RigidBody;


#endif 