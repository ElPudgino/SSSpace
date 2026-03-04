#ifndef MATH_UTIL
#define MATH_UTIL

#include "libs.h"

void Projection_Matrix(mat4 matrix,float aspect_ratio, float near, float far, float view_angle);

void Quaterion_ToMatrix(vec4 q, mat4 matrix);

void Translation_Matrix(double pos[3], mat4 matrix);

void Add_Position(double pos[3], double add[3]);

void Rotate_Position(double pos[3], vec4 q);

void Copy_Position(double src[3], double dest[3]);

#endif
