#include "input_registry.h"

void _FreeCamMove(int ind, int sign)
{
    double pos[3];
    Get_CameraPosition(pos);
    pos[ind] += sign ? -Get_FreeCameraVelocity() : Get_FreeCameraVelocity();
    Set_CameraPosition(pos);
}

void Ctrl_FreeCamForward() { _FreeCamMove(2, 0);}

void Ctrl_FreeCamBackward() { _FreeCamMove(2, 1);}

void Ctrl_FreeCamRight() { _FreeCamMove(0, 0);}

void Ctrl_FreeCamLeft() { _FreeCamMove(0, 1);}

void Ctrl_FreeCamUp() { _FreeCamMove(1, 1);}

void Ctrl_FreeCamDown() { _FreeCamMove(1, 0);}

int Register_Controls()
{
    Init_Controls();
    Add_Control(NULL, NULL, Ctrl_FreeCamForward, SDLK_W, "Forward");
    Add_Control(NULL, NULL, Ctrl_FreeCamBackward, SDLK_S, "Back");
    Add_Control(NULL, NULL, Ctrl_FreeCamRight, SDLK_D, "Right");
    Add_Control(NULL, NULL, Ctrl_FreeCamLeft, SDLK_A, "Left");
    Add_Control(NULL, NULL, Ctrl_FreeCamUp, SDLK_SPACE, "Up");
    Add_Control(NULL, NULL, Ctrl_FreeCamDown, SDLK_LSHIFT, "Down");
    return 0;
}

