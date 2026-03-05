#ifndef MATH_UTIL
#define MATH_UTIL

#include "libs.h"

void Projection_Matrix(mat4 matrix,float aspect_ratio, float near, float far, float view_angle);

void Quaterion_ToMatrix(vec4 q, mat4 matrix);

void Translation_Matrix(double pos[3], mat4 matrix);

void Add_dVec(double trg[3], double add[3]);

void Rotate_dVec(double vec[3], vec4 q);

void Copy_dVec(double src[3], double dest[3]);

void Mult_Rotations(float trg[4], float factor[4]);

void Copy_Rotation(float src[4], float dest[4]);

void Scalar_Mult(double vec[3], double s);

void Cast_ToDouble(float a[3], double b[3]);

#endif
