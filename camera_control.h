#ifndef CAMERA_CONTROL
#define CAMERA_CONTROL

#include "transform_utils.h"
#include "ship.h"

typedef enum
{
    CAM_MODE_FREE,
    CAM_MODE_ORBIT
} CameraMode;

typedef enum
{
    CAM_FOCUS_VIEW,
    CAM_FOCUS_GUI
} CameraFocus;

int Init_Camera();

int Destroy_Camera(EngineState* engineState);

void Set_CameraLocalPosition(double pos[3]);

void Get_CameraLocalPosition(double pos[3]);

void Get_CameraGlobalPosition(double dest[3]);

void Get_CameraGlobalRotation(float q[4]);

void Get_CameraMatrix(mat4 mtrx);

void Set_CameraOrbit(Ship* tr);

Ship* Get_CurrentCameraFocus();

Transform* Get_CameraParent();

double Get_OrbitCameraVelocity();

float Get_CameraAngularVelocity();

void Set_CameraAngles(float a[2]);

void Get_CameraAngles(float a[3]);

double Get_FreeCameraVelocity();

void Camera_RotateBy(float q[4]);

void Get_CameraForward(float dest[3]);

void Get_CameraForwardD(double dest[3]);

void Get_CameraUp(float dest[3]);

void Get_CameraUpD(double dest[3]);

void Get_CameraRight(float dest[3]);

void Get_CameraRightD(double dest[3]);


#endif