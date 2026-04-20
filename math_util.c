#include "math_util.h"

void Projection_Matrix(mat4 matrix,float aspect_ratio, float near, float far, float view_angle)
{
    glm_mat4_copy((mat4){{aspect_ratio/(float)tan(view_angle),0,0,0}, 
                    {0,1.0F/(float)tan(view_angle),0,0},
                    {0,0,-near/(far-near),(near*far)/(far-near)}, 
                    {0,0,1.0F,0}}, matrix);
    glm_mat4_transpose(matrix);
}

void Rotate_dVec(double pos[3], vec4 q)
{
    // TODO: redo this with double precision
    float pf[3];
    pf[0] = (float)pos[0];
    pf[1] = (float)pos[1];
    pf[2] = (float)pos[2];
    glm_quat_rotatev(q, pf, pf);
    pos[0] = (double)pf[0];
    pos[1] = (double)pf[1];
    pos[2] = (double)pf[2];
}

void Cast_ToDouble(float a[3], double b[3])
{
    b[0] = (double)a[0];
    b[1] = (double)a[1];
    b[2] = (double)a[2];
}

void Copy_dVec(double src[3], double dest[3])
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

void Add_dVec(double trg[3], double add[3])
{
    trg[0] += add[0];
    trg[1] += add[1];
    trg[2] += add[2];
}

void Scalar_Mult(double vec[3], double s)
{
    vec[0] *= s;
    vec[1] *= s;
    vec[2] *= s;
}

void Copy_Rotation(float src[4], float dest[4])
{
    glm_quat_copy(src, dest);
}

void Mult_Rotations(float first[4], float second[4])
{
    glm_quat_mul(second, first, first);
}

void Quaterion_ToMatrix(vec4 q, mat4 matrix)
{
    glm_quat_mat4(q, matrix);
}

void Translation_Matrix_s(float x, float y, float z, mat4 matrix)
{
    glm_mat4_copy((mat4){{1,0,0,0},{0,1,0,0},{0,0,1,0},{x,y,z,1}}, matrix);
}

void Translation_Matrix(double pos[3], mat4 matrix)
{
    glm_mat4_copy((mat4){{1,0,0,0},{0,1,0,0},{0,0,1,0},{(float)pos[0],(float)pos[1],(float)pos[2],1}}, matrix);
}

void BlockRotation_Matrix(char r, mat4 m)
{
    
}

void _numToChar(uint32_t n, char* c)
{
    switch (n)
    {
        case 0:
            *c = 'u';
        case 1:
            *c = 'd';
        case 2:
            *c = 'n';
        case 3:
            *c = 's';
        case 4:
            *c = 'e';
        case 5:
            *c = 'w';
        default:
            'x';
    }
}

char _charToNum(char d)
{
    switch (d)
    {
        case 'u':
            return 0;
        case 'd':
            return 1;
        case 'n':
            return 2;
        case 's':
            return 3;
        case 'e':
            return 4;
        case 'w':
            return 5;
        default:
            return 15;
    }
}

void BlockRotation_ToDirName(char r, char* up, char* north)
{
    uint32_t n = (uint32_t)r;
    n = n >> 4;
    _numToChar(n, up);
    n = (uint32_t)r;
    n = n % 16;
    _numToChar(n, north);
}

char DirName_ToBlockRotation(char up, char north)
{
    char res = 0;
    res = res | (_charToNum(up) << 4);
    res = res | _charToNum(north);
    return res;
}