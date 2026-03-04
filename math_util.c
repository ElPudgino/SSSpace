#include "math_util.h"

void Projection_Matrix(mat4 matrix,float aspect_ratio, float near, float far, float view_angle)
{
    glm_mat4_copy((mat4){{aspect_ratio/(float)tan(view_angle),0,0,0}, 
                    {0,1.0/tan(view_angle),0,0},
                    {0,0,-near/(far-near),(near*far)/(far-near)}, 
                    {0,0,1.0,0}}, matrix);
    glm_mat4_transpose(matrix);
}

void Rotate_Position(double pos[3], vec4 qf)
{
    double qd[4] = {(double)qf[0],(double)qf[1],(double)qf[2],(double)qf[3]};
}

void Copy_Position(double src[3], double dest[3])
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

void Add_Position(double pos[3], double add[3])
{
    pos[0] += add[0];
    pos[1] += add[1];
    pos[2] += add[2];
}

void Quaterion_ToMatrix(vec4 q, mat4 matrix)
{
    glm_quat_mat4(q, matrix);
}

void Translation_Matrix(double pos[3], mat4 matrix)
{
    glm_mat4_copy((mat4){{1,0,0,0},{0,1,0,0},{0,0,1,0},{(float)pos[0],(float)pos[1],(float)pos[2],1}}, matrix);
}

void Camera_Matrix(mat4 cameraTransform, mat4 dest)
{
    glm_mat4_inv(cameraTransform, dest);
}