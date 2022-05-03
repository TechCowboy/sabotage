extern char *title;

int read_joystick(int rotation)
{
    int fire = 0;
    // int s = eos_keyboard_status();
    // if (s != 0)
    //{
    //     sprintf(title, "status: %d %c", c, c);
    //     vprint(title, 1);
    //     return rotation;
    // }
    //int c = eos_read_keyboard();
    // sprintf(title, "c: %d %c", c, c);
    // vprint(title, 2);
    /*
    switch (c)
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
    */
    return rotation | fire;
}
