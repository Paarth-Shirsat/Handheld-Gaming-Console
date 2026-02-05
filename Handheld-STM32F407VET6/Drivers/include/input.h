#ifndef INPUT_H_
#define INPUT_H_

#include <stdbool.h>

typedef enum {
    INPUT_NONE = 0,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_ENTER,
    INPUT_ESC
} InputDirection;

typedef enum {
    CONTROL_JOYSTICK,
    CONTROL_MPU,
    CONTROL_BUTTONS
} ControlMode;

void Input_Init(void);
void Input_Update(void);
InputDirection Input_GetDirection(void);
bool Input_IsEnterPressed(void);
bool Input_IsEscPressed(void);
bool Input_IsJoystickButtonPressed(void);
ControlMode Input_GetControlMode(void);

#endif
