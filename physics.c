#include "physics.h"

// dir; point are in ship local space 
void Apply_Impulse(RigidBody* rb , vec3 vect, vec3 point)
{
    vec3 globalvect;
    vec3 interm;
    glm_quat_rotatev(rb->root->rotation, vect, globalvect);

    // Linear speed change
    glm_vec3_scale(globalvect, 1/rb->baseMass, interm);
    glm_vec3_add(rb->velocity, interm, rb->velocity);


    // Angular speed change
    // dL = r x dp 
    // dL = I*dw; dw = (I^-1)*dL

    glm_vec3_sub(point, rb->baseCenterOfMass, interm); // r
    glm_cross(interm, vect, interm); // dL
    glm_mat3_mulv(rb->invBaseInertiaTensor, interm, interm); // dw
    glm_quat_rotatev(rb->root->rotation, interm, interm);
    glm_vec3_add(rb->angularVelocity,interm,rb->angularVelocity);

}

void Apply_Velocities(RigidBody* rb, float dt)
{
    vec3 dp;
    glm_vec3_scale(rb->velocity, dt, dp);
    vec3 temp;
    float angle = glm_vec3_norm(rb->angularVelocity)*dt;

    // If angle is small: dpos = w*dt x r
    if (angle < GLM_PIf / 45.0)
    {
        glm_vec3_cross(rb->angularVelocity, rb->baseCenterOfMass, temp);
        glm_vec3_scale(temp, -dt, temp);
    }
    // Else rotate vector and substract
    else
    {
        versor q;
        glm_quat(q, angle, rb->angularVelocity[0], rb->angularVelocity[1], rb->angularVelocity[2]); // ! Is axis normalized?
        glm_quat_rotatev(q, rb->baseCenterOfMass, temp);
        glm_vec3_add(temp, rb->baseCenterOfMass, temp);
        glm_vec3_scale(temp, -dt, temp);
    }
    
    glm_vec3_add(dp, temp, dp);

    double ddp[3] = {(double)dp[0], (double)dp[1], (double)dp[2]};
    Add_dVec(rb->root->pos, ddp);

    glm_vec3_normalize_to(rb->angularVelocity, temp);
    versor q;
    glm_quat(q, angle, temp[0], temp[1], temp[2]);
    glm_quat_mul(q, rb->root->rotation, rb->root->rotation);
}