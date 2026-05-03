#include "input_registry.h"

static void (*moveVects[3])(double dest[3]) = {Get_CameraRightD, Get_CameraUpD, Get_CameraForwardD};
static int shiftHeld = 0;
static int ctrlHeld = 0;
static float defaultForce = 5000;

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

void Control_KeyPress(float dt) {ctrlHeld = 1;}

void Control_KeyRelease(float dt) {ctrlHeld = 0;}

void Shift_KeyPress(float dt) {shiftHeld = 1;}

void Shift_KeyRelease(float dt) {shiftHeld = 0;}

void Ctrl_FreeCamForward(float dt) { _FreeCamMove(2, 0);}

void Ctrl_FreeCamBackward(float dt) { _FreeCamMove(2, 1);}

void Ctrl_FreeCamRight(float dt) { _FreeCamMove(0, 0);}

void Ctrl_FreeCamLeft(float dt) { _FreeCamMove(0, 1);}

void Ctrl_FreeCamUp(float dt) { _FreeCamMove(1, 0);}

void Ctrl_FreeCamDown(float dt) { _FreeCamMove(1, 1);}

void Ctrl_OrbitCamForward(float dt)
{
    double pos[3];
    Get_CameraLocalPosition(pos);
    pos[2] += Get_OrbitCameraVelocity();
    Set_CameraLocalPosition(pos);
}

void Ctrl_OrbitCamBackward(float dt)
{
    double pos[3];
    Get_CameraLocalPosition(pos);
    pos[2] -= Get_OrbitCameraVelocity();
    Set_CameraLocalPosition(pos);
}

void force_forward(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[2] = defaultForce * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void force_backward(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[2] = -defaultForce * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void force_right(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[0] = defaultForce * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void force_left(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[0] = -defaultForce * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void force_up(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[1] = -defaultForce * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void force_down(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[1] = defaultForce * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void torque_right(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[0] = defaultForce/2.0 * dt;
    p[2] = 1;
    Apply_Impulse(&s->rb, dir, p);
    p[2] = -1;
    dir[0] = -defaultForce/2.0 * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void torque_left(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[0] = defaultForce/2.0 * dt;
    p[2] = -1;
    Apply_Impulse(&s->rb, dir, p);
    p[2] = 1;
    dir[0] = -defaultForce/2.0 * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void torque_up(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[1] = defaultForce/2.0 * dt;
    p[2] = -1;
    Apply_Impulse(&s->rb, dir, p);
    p[2] = 1;
    dir[1] = -defaultForce/2.0 * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void torque_down(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[1] = defaultForce/2.0 * dt;
    p[2] = 1;
    Apply_Impulse(&s->rb, dir, p);
    p[2] = -1;
    dir[1] = -defaultForce/2.0 * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void torque_twist_cw(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[0] = defaultForce/2.0 * dt;
    p[1] = -1;
    Apply_Impulse(&s->rb, dir, p);
    p[1] = 1;
    dir[0] = -defaultForce/2.0 * dt;
    Apply_Impulse(&s->rb, dir, p);
}

void torque_twist_countercw(float dt)
{
    Ship* s = Get_CurrentCameraFocus();
    vec3 p = {};
    vec3 dir = {};
    dir[0] = defaultForce/2.0 * dt;
    p[1] = 1;
    Apply_Impulse(&s->rb, dir, p);
    p[1] = -1;
    dir[0] = -defaultForce/2.0 * dt;
    Apply_Impulse(&s->rb, dir, p);
}

int Register_Controls()
{
    Init_Controls();
    Add_Control(Control_KeyPress, Control_KeyRelease, NULL, SDLK_LCTRL, "LControl");
    Add_Control(Shift_KeyPress, Shift_KeyRelease, NULL, SDLK_LSHIFT, "LShift");
    Add_Control(NULL, NULL, Ctrl_FreeCamForward, SDLK_W, "Forward");
    Add_Control(NULL, NULL, Ctrl_FreeCamBackward, SDLK_S, "Back");
    Add_Control(NULL, NULL, Ctrl_FreeCamRight, SDLK_D, "Right");
    Add_Control(NULL, NULL, Ctrl_FreeCamLeft, SDLK_A, "Left");
    Add_Control(NULL, NULL, Ctrl_FreeCamUp, SDLK_SPACE, "Up");
    Add_Control(NULL, NULL, Ctrl_FreeCamDown, SDLK_LSHIFT, "Down");
    Add_Control(NULL, NULL, Ctrl_OrbitCamForward, SDLK_Z, "OrbitForward");
    Add_Control(NULL, NULL, Ctrl_OrbitCamBackward, SDLK_X, "OrbitBackward");

    Add_Control(NULL, NULL, force_forward, SDLK_W, "testForward");
    Add_Control(NULL, NULL, force_backward, SDLK_S, "testBackward");
    Add_Control(NULL, NULL, force_right, SDLK_D, "testRight");
    Add_Control(NULL, NULL, force_left, SDLK_A, "testLeft");
    Add_Control(NULL, NULL, force_up, SDLK_SPACE, "testUp");
    Add_Control(NULL, NULL, force_down, SDLK_LSHIFT, "testDown");

    Add_Control(NULL, NULL, torque_up, SDLK_UP, "testT_Up");
    Add_Control(NULL, NULL, torque_down, SDLK_DOWN, "testT_Down");
    Add_Control(NULL, NULL, torque_right, SDLK_RIGHT, "testT_Right");
    Add_Control(NULL, NULL, torque_left, SDLK_LEFT, "testT_Left");
    Add_Control(NULL, NULL, torque_twist_cw, SDLK_E, "testT_CW");
    Add_Control(NULL, NULL, torque_twist_countercw, SDLK_Q, "testT_CounterCW");

    return 0;
}

