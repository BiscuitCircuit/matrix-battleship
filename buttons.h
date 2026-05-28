#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

/* BUTTON MAPPING
* BLACK = D9 = CONFIRM, ROTATE
* WHITE = D10 = RIGHT
* BLUE = D11 = LEFT
* GREEN = D12 = DOWN
* YELLOW = D13 = UP
*/

#define BTN_UP      D13
#define BTN_DOWN    D12
#define BTN_LEFT    D11
#define BTN_RIGHT   D10
#define BTN_CONFIRM D9

#define NUM_BUTTONS 5

#define DEBOUNCE_MS 50
#define LONG_PRESS_MS 600

enum ButtonEvent
{
    BTN_NONE = 0,
    BTN_SHORT_PRESS,
    BTN_LONG_PRESS
};

enum ButtonID
{
    ID_UP = 0,
    ID_DOWN,
    ID_LEFT,
    ID_RIGHT,
    ID_CONFIRM
};

struct ButtonState
{
    uint8_t pin;
    bool lastRaw;
    bool isPressed;
    uint32_t lastChangeTime;
    uint32_t pressedAt;
    bool longFired;
    ButtonEvent event;
};

void buttons_init();
void buttons_update();
ButtonEvent buttons_getEvent(ButtonID id);
bool buttons_isHeld(ButtonID id);

#endif