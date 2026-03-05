#include "camera_control.h"

Transform* CameraTransform = NULL;
double BaseCameraVelocity = 0.015;
double BaseCameraAngularVelocity = 0.01;
// usually we dont want to rotate camera around the forward axis, 
// these are euler angles
float CameraAngles[3] = {0,0,0};
CameraMode CurrentCameraMode = CAM_MODE_FREE;
 
void Get_CameraMatrix(mat4 mtrx)
{
    float q[4];
    Get_CameraGlobalRotation(q);
    glm_quat_inv(q, q);
    Quaterion_ToMatrix(q, mtrx);
}

// For rendering always keep camera pos at 0
// apply translation with doubles and not matrices
// this means substract camera pos from objects that are not parented
void Get_CameraGlobalPosition(double dest[3])
{
    assert(CameraTransform);
    if (CurrentCameraMode == CAM_MODE_FREE)
    {
        Copy_dVec(CameraTransform->pos, dest);
        Transform* next = CameraTransform->parent;
        while (next)
        {
            Rotate_dVec(dest, next->rotation);   
            Add_dVec(dest, next->pos);
            next = next->parent;
        }
    }
    else if (CurrentCameraMode == CAM_MODE_ORBIT)
    {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = CameraTransform->pos[2];
        Rotate_dVec(dest, CameraTransform->rotation);
        Transform* next = CameraTransform->parent;
        while (next)
        {
            Rotate_dVec(dest, next->rotation);   
            Add_dVec(dest, next->pos);
            next = next->parent;
        }
    }
}

void Set_CameraOrbit(Transform* tr)
{
    assert(tr);
    CameraTransform->parent = tr;
    CameraTransform->pos[0] = 0;
    CameraTransform->pos[1] = 0;
    CameraTransform->pos[2] = -16;
    CurrentCameraMode = CAM_MODE_ORBIT;
}

void Get_CameraGlobalRotation(float q[4])
{
    Copy_Rotation(CameraTransform->rotation, q);
    Transform* next = CameraTransform->parent;
    while (next)
    {
        Mult_Rotations(q, next->rotation);
        next = next->parent;
    }
}

void Get_CameraAngles(float a[3])
{   
    a[0] = CameraAngles[1];
    a[1] = CameraAngles[0];
    a[2] = CameraAngles[2];
}

void Set_CameraAngles(float a[2])
{
    if (a[0] > GLM_PIf/2.01F) a[0] = GLM_PIf/2.01F;
    if (a[0] < -GLM_PIf/2.01F) a[0] = -GLM_PIf/2.01F;

    CameraAngles[1] = a[0];
    CameraAngles[0] = a[1];

    glm_euler_yxz_quat(a, CameraTransform->rotation);
}

void Get_CameraForward(float dest[3])
{
    Get_TransformForward(CameraTransform, dest);
}

void Get_CameraForwardD(double dest[3])
{
    float p[3];
    Get_CameraForward(p);
    Cast_ToDouble(p, dest);
}

void Get_CameraUp(float dest[3])
{
    Get_TransformUp(CameraTransform, dest);
}

void Get_CameraUpD(double dest[3])
{
    float p[3];
    Get_CameraUp(p);
    Cast_ToDouble(p, dest);
}

void Get_CameraRight(float dest[3])
{
    Get_TransformRight(CameraTransform, dest);
}

void Get_CameraRightD(double dest[3])
{
    float p[3];
    Get_CameraRight(p);
    Cast_ToDouble(p, dest);
}

void Set_CameraLocalPosition(double pos[3])
{
    assert(CameraTransform);
    Copy_dVec(pos,  CameraTransform->pos);
}

void Get_CameraLocalPosition(double pos[3])
{
    Copy_dVec(CameraTransform->pos, pos);
}

double Get_OrbitCameraVelocity()
{
    if (CurrentCameraMode != CAM_MODE_ORBIT) return 0;
    return BaseCameraVelocity*2.0;
}

double Get_FreeCameraVelocity()
{
    if (CurrentCameraMode != CAM_MODE_FREE) return 0;
    return BaseCameraVelocity;
}

float Get_CameraAngularVelocity()
{
    return BaseCameraAngularVelocity;
}

int Init_Camera()
{
    CameraTransform = (Transform*)calloc(1, sizeof(Transform));
    if (!CameraTransform) return 1;
    glm_euler_yxz_quat(CameraAngles,CameraTransform->rotation);
    return 0;
}

int Destroy_Camera(EngineState* engineState)
{
    free(CameraTransform);
    return 1;
}