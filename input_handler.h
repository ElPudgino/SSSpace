#ifndef INPUT_HANDLER
#define INPUT_HANDLER

#include "camera_control.h"

// used for buttons
typedef void (*InputAction)();

typedef struct _ControlKey
{
    SDL_Keycode key;
    int state;
    InputAction OnPressed; 
    InputAction OnReleased;  
    InputAction WhileHeld;
    char const* Name;
} ControlKey;

typedef struct _Controls
{
    ControlKey* keys;
    uint32_t keyCount;
    uint32_t keyCap;
} Controls;

void Add_Control(InputAction press, InputAction release, InputAction hold, SDL_Keycode defaultkey, char const* name);

int Destroy_Controls(EngineState* engineState);

void Init_Controls();

void Process_PersistentInput();

void Process_KeyboardInput(SDL_Event event);

void Process_MouseInput(SDL_Event event);

#endif