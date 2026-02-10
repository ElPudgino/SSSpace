#include "math_util.h"

void Projection_Matrix(mat4 matrix,float aspect_ratio, float near, float far, float view_angle)
{
    glm_mat4_copy((mat4){{aspect_ratio/tan(view_angle),0,0,0}, 
                    {0,1.0/tan(view_angle),0,0},
                    {0,0,-near/(far-near),(near*far)/(far-near)}, 
                    {0,0,1.0,0}}, matrix);
    glm_mat4_transpose(matrix);
}

