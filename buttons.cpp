#include "buttons.h"

static ButtonState buttons[NUM_BUTTONS] = {
    {BTN_UP, HIGH, false, 0, 0, false, BTN_NONE},
    {BTN_DOWN, HIGH, false, 0, 0, false, BTN_NONE},
    {BTN_LEFT, HIGH, false, 0, 0, false, BTN_NONE},
    {BTN_RIGHT, HIGH, false, 0, 0, false, BTN_NONE},
    {BTN_CONFIRM, HIGH, false, 0, 0, false, BTN_NONE}};

void buttons_init()
{
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        pinMode(buttons[i].pin, INPUT_PULLUP);
        buttons[i].lastRaw = digitalRead(buttons[i].pin);
        buttons[i].isPressed = false;
        buttons[i].lastChangeTime = millis();
        buttons[i].pressedAt = 0;
        buttons[i].longFired = false;
        buttons[i].event = BTN_NONE;
    }
}

void buttons_update(){
    uint32_t now = millis();

    for (int i = 0; i < NUM_BUTTONS; i++){
        bool raw = digitalRead(buttons[i].pin);

        if (raw != buttons[i].lastRaw){
            buttons[i].lastRaw = raw;
            buttons[i].lastChangeTime = now;
        }

        if ((now - buttons[i].lastChangeTime) >= DEBOUNCE_MS){
            bool currentlyPressed = (raw == LOW);

            if (currentlyPressed && !buttons[i].isPressed){
                buttons[i].isPressed = true;
                buttons[i].pressedAt = now;
                buttons[i].longFired = false;
            }
            else if (!currentlyPressed && buttons[i].isPressed){
                buttons[i].isPressed = false;

                if (!buttons[i].longFired)
                {
                    buttons[i].event = BTN_SHORT_PRESS;
                }
            }
            else if (currentlyPressed && buttons[i].isPressed)
            {
                if (!buttons[i].longFired &&
                    (now - buttons[i].pressedAt >= LONG_PRESS_MS))
                {
                    buttons[i].event = BTN_LONG_PRESS;
                    buttons[i].longFired = true;
                }
            }
        }
    }
}

ButtonEvent buttons_getEvent(ButtonID id)
{
    ButtonEvent e = buttons[id].event;
    buttons[id].event = BTN_NONE;
    return e;
}

bool buttons_isHeld(ButtonID id)
{
    return buttons[id].isPressed;
}