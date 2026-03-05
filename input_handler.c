#include "input_handler.h"

Controls controls;
#define TRY_ACTION(a) if (a) a()

void Init_Controls()
{
    controls.keys = (ControlKey*)calloc(4, sizeof(ControlKey));
    controls.keyCap = 4;
}

int Destroy_Controls(EngineState* engineState)
{
    free(controls.keys);
    return 1;
}

void Add_Control(InputAction press, InputAction release, InputAction hold, SDL_Keycode defaultkey, char const* name)
{
    if (controls.keyCount == controls.keyCap)
    {
        controls.keys = (ControlKey*)realloc(controls.keys, 2 * controls.keyCap * sizeof(ControlKey));
        controls.keyCap *= 2;
    }
    controls.keys[controls.keyCount] = (ControlKey){.key = defaultkey, .OnPressed = press, .OnReleased = release, .WhileHeld = hold, .Name = name};
    controls.keyCount++;
}

void Process_PositionalInput(float data[4])
{
    float a[3] = {};
    Get_CameraAngles(a);
    float s = Get_CameraAngularVelocity();
    a[0] += -data[1] * s; // To rotate camera up and down we use x axis
    a[1] += data[0] * s;
    Set_CameraAngles(a);
}

void Process_MouseInput(SDL_Event event)
{
    float vals[4]; // xrel, yrel, xabs, yabs
    vals[0] = event.motion.xrel;
    vals[1] = event.motion.yrel;
    vals[2] = event.motion.x;
    vals[3] = event.motion.y;
    Process_PositionalInput(vals);
}

// A single key could be bound to several controls
void Process_KeyboardInput(SDL_Event event)
{
    for (int i = 0; i < controls.keyCount; i++) 
    {
        ControlKey ck = controls.keys[i];
        if (event.key.key != ck.key) continue;
        if (event.type == SDL_EVENT_KEY_DOWN && !ck.state) {TRY_ACTION(ck.OnPressed); controls.keys[i].state = 1;}
        else if (event.type == SDL_EVENT_KEY_UP) {TRY_ACTION(ck.OnReleased); controls.keys[i].state = 0;}
    }
}

void Process_PersistentInput()
{
    ControlKey key = {};
    for (int i = 0; i < controls.keyCount; i++)
    {
        key = controls.keys[i];
        if (key.state) TRY_ACTION(key.WhileHeld);
    }
}