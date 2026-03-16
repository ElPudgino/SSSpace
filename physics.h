#ifndef PHYSICAL_BODY
#define PHYSICAL_BODY

#include "math_util.h"
#include "basic_objects.h" 

typedef struct _RigidBody
{
    Transform* root;

    mat3 baseInertiaTensor; // Only main hull, local space    
    mat3 invBaseInertiaTensor;

    float baseMass;
    float baseVolume;
    vec3 baseCenterOfMass; // Only main hull
    // Offset from pivot point (0 if center is at pivot); In local space

    vec3 velocity; // Global space
    vec3 angularVelocity; // Global space
    // ! Rotation happens around center of mass, not base pivot:
    // Need the pivot point to orbit the center
     
    // ! Center of mass can change depending on cargo and other stuff like docked ships
    // 
} RigidBody;

void Apply_Impulse(RigidBody* rb , vec3 vect, vec3 point);

void Apply_Velocities(RigidBody* rb, float dt);

#endif 