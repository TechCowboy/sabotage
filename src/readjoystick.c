#include <stdio.h>
#include <eos.h>
#include "misc.h"
#include "charctrl.h"
static GameControllerData cont;
static unsigned char joystick = 0;
static unsigned char button = 0;
static unsigned char keypad = 0;

#define READ_CONTROLLER_1 1
#define READ_CONTROLLER_2 2
#define READ_SPINNER      128

#define UP    1
#define RIGHT 2
#define DOWN  4
#define LEFT  8

int read_joystick(int rotation)
{
    char temp[90];
    int fire = 0;

    rotation = rotation & (~16384);

    eos_read_game_controller(READ_CONTROLLER_1 | READ_CONTROLLER_2, &cont);
    joystick = cont.joystick1 | cont.joystick2;  // use value from either joystick
    button = cont.joystick1_button_left | cont.joystick1_button_right | cont.joystick2_button_left | cont.joystick2_button_right;
    keypad = cont.joystick1_keypad;

    switch(joystick)
    {

        case LEFT: 
            rotation--;
            break;
        case RIGHT:
            rotation++;
            break;
        default:
            break;
    }

    rotation = MAX(0, rotation);
    rotation = MIN(6, rotation);

    if (button)
        fire = 16384;

    return rotation | fire;
}
