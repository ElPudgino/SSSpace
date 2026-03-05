#include "input_registry.h"

static void (*moveVects[3])(double dest[3]) = {Get_CameraRightD, Get_CameraUpD, Get_CameraForwardD};
void _FreeCamMove(int ind, int sign)
{
    double pos[3];
    Get_CameraLocalPosition(pos);
    double s = sign ? -Get_FreeCameraVelocity() : Get_FreeCameraVelocity();
    double vel[3];
    moveVects[ind](vel);
    Scalar_Mult(vel, s);
    Add_dVec(pos, vel);
    Set_CameraLocalPosition(pos);
}

void Ctrl_FreeCamForward() { _FreeCamMove(2, 0);}

void Ctrl_FreeCamBackward() { _FreeCamMove(2, 1);}

void Ctrl_FreeCamRight() { _FreeCamMove(0, 0);}

void Ctrl_FreeCamLeft() { _FreeCamMove(0, 1);}

void Ctrl_FreeCamUp() { _FreeCamMove(1, 0);}

void Ctrl_FreeCamDown() { _FreeCamMove(1, 1);}

void Ctrl_OrbitCamForward()
{
    double pos[3];
    Get_CameraLocalPosition(pos);
    pos[2] += Get_OrbitCameraVelocity();
    Set_CameraLocalPosition(pos);
}

void Ctrl_OrbitCamBackward()
{
    double pos[3];
    Get_CameraLocalPosition(pos);
    pos[2] -= Get_OrbitCameraVelocity();
    Set_CameraLocalPosition(pos);
}

int Register_Controls()
{
    Init_Controls();
    Add_Control(NULL, NULL, Ctrl_FreeCamForward, SDLK_W, "Forward");
    Add_Control(NULL, NULL, Ctrl_FreeCamBackward, SDLK_S, "Back");
    Add_Control(NULL, NULL, Ctrl_FreeCamRight, SDLK_D, "Right");
    Add_Control(NULL, NULL, Ctrl_FreeCamLeft, SDLK_A, "Left");
    Add_Control(NULL, NULL, Ctrl_FreeCamUp, SDLK_SPACE, "Up");
    Add_Control(NULL, NULL, Ctrl_FreeCamDown, SDLK_LSHIFT, "Down");
    Add_Control(NULL, NULL, Ctrl_OrbitCamForward, SDLK_W, "OrbitForward");
    Add_Control(NULL, NULL, Ctrl_OrbitCamBackward, SDLK_S, "OrbitBackward");
    return 0;
}

