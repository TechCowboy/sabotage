#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "vdp_registers.h"
#include <eos.h>

extern char *title;

void init_keyboard(void)
{
    eos_start_read_keyboard();
}

int read_keyboard(int rotation)
{
    int fire=0;

    rotation = rotation & (~16384);
    int key = eos_end_read_keyboard();
    eos_start_read_keyboard();
    if (key > 1)
    {
        switch (key)
        {
        case 'j':
        case 'J':
            rotation--;
            break;
        case 'k':
        case 'K':
            rotation++;
            break;
        default:
            fire = 16384;
            break;
        }
        rotation = MAX(0, rotation);
        rotation = MIN(6, rotation);
    }

    return rotation | fire;
}
