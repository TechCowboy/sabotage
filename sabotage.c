#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msx.h>
#include <msx/gfx.h>
#include <arch/z80.h>
#include "charset.h"
#include "colorset.h"
#include "spriteset.h"
#include "vdp_registers.h"
#include "colorset.h"
#include "graphics.h"

#define MIN(x, y) x < y ? x : y
#define MAX(x, y) x > y ? x : y

unsigned char length_check[] = { "1234567890123456789012345678901234567890"};
unsigned char title[80];


void debug(char *message, int line)
{
    vwrite("                                                 ", VRAM_NAME_TABLE + line * total_columns, total_columns);
    vwrite(message, VRAM_NAME_TABLE+line*total_columns, strlen(message));
    //getchar();
}

int introduction(int group)
{
    unsigned char intro[] =
        "     ************SABOTAGE***********    "
        "      Based on a game by Mark Allen     "
        "     Coleco Adaption by Norman Davie    "
        "                                        "
        "The object of  SABOTAGE  is  to score as"
        "many points as possible before the enemy"
        "destroys your gun  emplacement. Your gun"
        "may be destroyed either by a hit from an"
        "enemy bomb  or by  SABOTAGE  when enough"
        "paratroopers  have reached  the  ground."
        "The gun is controlled by the joystick or"
        "the  keyboard.  If  you  are  using  the"
        "keyboard,  pressing 'D'  will  move  the"
        "gun counter-clockwise and  'F' will move"
        "it clockwise.  Any other key will fire a"
        "shell.  FIRING A SHELL  COSTS ONE POINT."
        "          TARGET     POINTS             "
        "          ======     ======             "
        "          BOMB           25             "
        "          HELICOPTER      5             "
        "          JET             5             "
        "          PARATROOPER     2             "
        "                                        "
        "   DO YOU WANT  STEERABLE SHELLS (Y/N)? ";
 
    for (int x = 0; x < strlen(intro); x++)
    {
        intro[x] = intro[x] + 256*group;
    }
    vwrite(intro, VRAM_NAME_TABLE, MIN(strlen(intro), total_columns*total_rows));

    int c = getchar();

    return c;
}


void create_text_ground(void)
{

    int y = total_rows-1;
    int x;
    
    for (x=0; x<total_columns; x++)
        vpoke(VRAM_NAME_TABLE+y*total_columns+x, CHAR_GROUND);

}

void create_text_turret(void)
{

    int y = total_rows - 1;
    int x;

    for (y = total_rows - 2; y < total_rows + 1; y++)
    {
        x = total_columns / 2 - 1;
        if (y == total_columns - 1)
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_LEFT);
        else
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);

        vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);

        if (y == total_columns - 1)
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_RIGHT);
        else
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);
    }
}

#define JUMPED 0
#define JUMPED1 1
#define JUMPED2 2
#define JUMPED3 3
#define FLOATING 4
#define FALLING 5
#define GROUNDED 6

int parachuter(int sprite_number, SPRITE_STATE info)
{
    static bool fail_sprite_flag = false;

    if (info.state == -1)
    {
        int y1 = 200;
        int x1 = 0;
        for(int s= 0; s<3; s++)
        {
            sprite_attributes[s].x = x1;
            sprite_attributes[s].y = y1;
            x1 += 8;
        }
    } else
    {

        if (info.state <= JUMPED3)
        {
            // hide chute
            sprite_attributes[sprite_number].y     = 192 + 8;
            sprite_attributes[sprite_number + 1].y = 192 + 8;

            sprite_attributes[sprite_number + 2].sprite_pattern = GROUND_MAN;
            sprite_attributes[sprite_number + 2].color_code = COLOR_DARK_BLUE;

            sprite_attributes[sprite_number + 2].x = info.x + 4;
            sprite_attributes[sprite_number + 2].y = info.y;

        } else
        {
            // normal parachute
            if (info.state == FLOATING)
            {
                sprite_attributes[sprite_number].sprite_pattern     = CHUTE_LEFT;
                sprite_attributes[sprite_number + 1].sprite_pattern = CHUTE_RIGHT;
                sprite_attributes[sprite_number + 2].sprite_pattern = CHUTE_MAN;
            }
            
            // broken parachute
            if (info.state == FALLING)
            {
                sprite_attributes[sprite_number].sprite_pattern     = fail_sprite_flag ? CHUTE_LEFT_FAIL  : CHUTE_LEFT_FAIL2;
                sprite_attributes[sprite_number + 1].sprite_pattern = fail_sprite_flag ? CHUTE_RIGHT_FAIL : CHUTE_RIGHT_FAIL2;
                sprite_attributes[sprite_number + 2].sprite_pattern = GROUND_MAN;
            }

            sprite_attributes[sprite_number].color_code     = COLOR_WHITE;
            sprite_attributes[sprite_number].x = info.x;
            sprite_attributes[sprite_number].y = info.y;

            sprite_attributes[sprite_number + 1].color_code = COLOR_WHITE;
            sprite_attributes[sprite_number + 1].x = info.x+8;
            sprite_attributes[sprite_number + 1].y = info.y;

            sprite_attributes[sprite_number + 2].color_code = COLOR_DARK_BLUE;
            sprite_attributes[sprite_number + 2].x          = info.x + 4;
            sprite_attributes[sprite_number + 2].y          = info.y + 8;
        }

    }

    //debug("after vwrite ");
    fail_sprite_flag = !fail_sprite_flag;

    return 3; // 3 sprites used
}


int parachuter_crash(int sprite_number, SPRITE_STATE info)
{
    static bool fail_sprite_flag = true;

    sprite_attributes[sprite_number].sprite_pattern = fail_sprite_flag ? CHUTE_LEFT_FAIL : CHUTE_LEFT_FAIL2;

    sprite_attributes[sprite_number].color_code = COLOR_DARK_RED;

    sprite_attributes[sprite_number].x = info.x;
    sprite_attributes[sprite_number].y = info.y;

    sprite_attributes[sprite_number + 1].sprite_pattern = fail_sprite_flag ? CHUTE_RIGHT_FAIL : CHUTE_RIGHT_FAIL2;
    sprite_attributes[sprite_number + 1].color_code = COLOR_DARK_RED;

    sprite_attributes[sprite_number + 1].x = info.x + 8;
    sprite_attributes[sprite_number + 1].y = info.y;

    sprite_attributes[sprite_number + 2].sprite_pattern = CHUTE_MAN;
    sprite_attributes[sprite_number + 2].color_code = COLOR_DARK_RED;

    sprite_attributes[sprite_number + 2].x = info.x + 4;
    sprite_attributes[sprite_number + 2].y = info.y + 8;


    fail_sprite_flag = !fail_sprite_flag;

    return 3; // 3 sprites used
}

int jet(int sprite_number, SPRITE_STATE info)
{
    static bool fail_sprite_flag = true;

    sprite_attributes[sprite_number].sprite_pattern = info.going_left ? LJET_FRONT : JET_END;

    sprite_attributes[sprite_number].color_code = COLOR_BLACK;

    sprite_attributes[sprite_number].x = info.x;
    sprite_attributes[sprite_number].y = info.y;

    sprite_attributes[sprite_number + 1].sprite_pattern = info.going_left ? LJET_MID : JET_MID;
    sprite_attributes[sprite_number + 1].color_code = COLOR_BLACK;

    sprite_attributes[sprite_number + 1].x = info.x + 8;
    sprite_attributes[sprite_number + 1].y = info.y;

    sprite_attributes[sprite_number + 2].sprite_pattern = info.going_left ? LJET_END : JET_FRONT;
    sprite_attributes[sprite_number + 2].color_code = COLOR_BLACK;

    sprite_attributes[sprite_number + 2].x = info.x + 16;
    sprite_attributes[sprite_number + 2].y = info.y;

    fail_sprite_flag = !fail_sprite_flag;

    return 3; // 3 sprites used
}


int bomb(int sprite_number, SPRITE_STATE info)
{
    static bool fail_sprite_flag = true;


    sprite_attributes[sprite_number].sprite_pattern = BOMB;

    sprite_attributes[sprite_number].color_code = COLOR_LIGHT_RED;

    sprite_attributes[sprite_number].x = info.x;
    sprite_attributes[sprite_number].y = info.y;

    fail_sprite_flag = !fail_sprite_flag;

    return 1;
}

int shot(int sprite_number, SPRITE_STATE info)
{
    static bool fail_sprite_flag = true;


    sprite_attributes[sprite_number].sprite_pattern = SHOT;

    sprite_attributes[sprite_number].color_code = COLOR_MAGENTA;

    sprite_attributes[sprite_number].x = info.x;
    sprite_attributes[sprite_number].y = info.y;


    fail_sprite_flag = !fail_sprite_flag;

    return 1;
}

int helicopter(int sprite_number, SPRITE_STATE info)
{
    static bool alternate = true;

    if (info.going_left)
    {

        // top
        sprite_attributes[sprite_number].x = info.x;
        sprite_attributes[sprite_number].y = info.y;
        sprite_attributes[sprite_number].color_code = (alternate || info.x < 8) ? COLOR_TRANSPARENT : COLOR_WHITE;
        sprite_attributes[sprite_number].sprite_pattern = CHOPTER_BLADE_LEFT;

        sprite_attributes[sprite_number + 1].x = info.x + 8;
        sprite_attributes[sprite_number + 1].y = info.y;
        sprite_attributes[sprite_number + 1].color_code = (info.x+8 < 8) ? COLOR_TRANSPARENT : COLOR_WHITE;
        
        sprite_attributes[sprite_number + 1].sprite_pattern = alternate ? CHOPTER_BLADE_RIGHT_MID : CHOPTER_BLADE_LEFT_MID;

        sprite_attributes[sprite_number + 2].x = info.x + 16;
        sprite_attributes[sprite_number + 2].y = info.y;
        sprite_attributes[sprite_number + 2].color_code = (alternate || info.x+16 < 8) ? COLOR_WHITE : COLOR_TRANSPARENT;
        sprite_attributes[sprite_number + 2].sprite_pattern = CHOPTER_BLADE_RIGHT;

        // mid
        sprite_attributes[sprite_number + 3].x = info.x;
        sprite_attributes[sprite_number + 3].y = info.y + 8;
        sprite_attributes[sprite_number + 3].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 3].sprite_pattern = LCHOPTER_FRONT;

        sprite_attributes[sprite_number + 4].x = info.x + 8;
        sprite_attributes[sprite_number + 4].y = info.y + 8;
        sprite_attributes[sprite_number + 4].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 4].sprite_pattern = LCHOPTER_MID;

        sprite_attributes[sprite_number + 5].x = info.x + 16;
        sprite_attributes[sprite_number + 5].y = info.y + 8;
        sprite_attributes[sprite_number + 5].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 5].sprite_pattern = alternate ? LCHOPTER_ROTOR_UP : LCHOPTER_ROTOR_SIDE;
    } 
    else
    {

        // top
        sprite_attributes[sprite_number].x = info.x;
        sprite_attributes[sprite_number].y = info.y;
        sprite_attributes[sprite_number].color_code = alternate ? COLOR_TRANSPARENT : COLOR_WHITE;
        sprite_attributes[sprite_number].sprite_pattern = CHOPTER_BLADE_LEFT;

        sprite_attributes[sprite_number + 1].x = info.x + 8;
        sprite_attributes[sprite_number + 1].y = info.y;
        sprite_attributes[sprite_number + 1].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 1].sprite_pattern = alternate ? CHOPTER_BLADE_RIGHT_MID : CHOPTER_BLADE_LEFT_MID;

        sprite_attributes[sprite_number + 2].x = info.x + 16;
        sprite_attributes[sprite_number + 2].y = info.y;
        sprite_attributes[sprite_number + 2].color_code = alternate ? COLOR_WHITE : COLOR_TRANSPARENT;
        sprite_attributes[sprite_number + 2].sprite_pattern = CHOPTER_BLADE_RIGHT;

        // mid
        sprite_attributes[sprite_number + 3].x = info.x;
        sprite_attributes[sprite_number + 3].y = info.y + 8;
        sprite_attributes[sprite_number + 3].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 3].sprite_pattern = alternate ? CHOPTER_ROTOR_UP : CHOPTER_ROTOR_SIDE;

        sprite_attributes[sprite_number + 4].x = info.x + 8;
        sprite_attributes[sprite_number + 4].y = info.y + 8;
        sprite_attributes[sprite_number + 4].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 4].sprite_pattern = CHOPTER_MID;

        sprite_attributes[sprite_number + 5].x = info.x + 16;
        sprite_attributes[sprite_number + 5].y = info.y + 8;
        sprite_attributes[sprite_number + 5].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 5].sprite_pattern = CHOPTER_FRONT;
    }

    alternate = ! alternate;

    return 6;
}

void end()
{
    mode_text();
    clr(' ');
    strcpy(title, "The End.");
    vwrite(title, VRAM_NAME_TABLE, strlen(title));
}

void delay(int x)
{
    for(int z=0; z<x; z++)
    {
        for (int z2=0; z2<100; z2++)
            ;
    }
}

/*
color mode ii

The first character is the top-left one, the second the one to the right of
it. Character no. 32 the first character of the second row. There are three
colour and three pattern tables. The character number is as follows:

The first 8 rows: byte from PN + 000h
The middle 8 rows: byte from PN + 100h
The bottom 8 rows: byte from PN + 200h

Each character is 8 x 8. The character number refers to an character pattern 
in the PG, which has 768 characters patterns (three distinct tables). For the 
first 8 rows of the screen, the first 256 entries are used. For the second 8 
rows, the second 256 patterns, and for the last 8 rows the third 256 patterns 
(depending on the value of control register #4, see below). So the entire PG is
3 x 256 x 8 = 6144 bytes. Each byte is a pixel line of the character (top 
first). If a bit is set, the foreground colour in the CT is used, otherwise 
the background colour in the CT.
*/

void test_char_color(void)
{
    int x;
    int y = 0;

    int color1 = SHIFTED_COLOR_CYAN | COLOR_DARK_RED;
    int color2 = SHIFTED_COLOR_DARK_RED  | COLOR_CYAN;
    int color3 = SHIFTED_COLOR_DARK_YELLOW | COLOR_LIGHT_BLUE;
    vpoke(VRAM_COLOR_TABLE,        color1);
    vpoke(VRAM_COLOR_TABLE + 2048, color2);
    vpoke(VRAM_COLOR_TABLE + 4096, color3);

    for (int z=0; z < 4096; z += 2048)
    {
        for(y = 0; y < total_rows; y += 2)
        {
            for (x=0; x < total_columns; x += 2)
            {
                vpoke(VRAM_NAME_TABLE+y*total_columns + x, z);
            }
        }
    }
}

/*
int collision_detect(int sprite_number)
{
    int min_x=257, 
        max_x=-1, 
        min_y=193, 
        max_y=-1;
    int sprite_collision = -1;

    if (sprite_number >= 0)
    {
        for(int s=0; s<32; s++)
        {
            if (sprite_list[s] == -1)
                break;

            if (sprite_list[s] == sprite_number)
            {
                min_x = MIN(min_x, (int) sprite_attributes[s].x);
                max_x = MAX(max_x, (int) sprite_attributes[s].x+8);
                min_y = MIN(min_y, (int) sprite_attributes[s].y);
                max_y = MAX(max_y, (int) sprite_attributes[s].y+8);
            }
        }

        for(int s=0; s<32; s++)
        {
            if (sprite_list[s] == -1)
                break;

            if (sprite_attributes[s].y == SPRITE_TERMINATOR)
                break;
            
            if (sprite_list[s] == sprite_number)
                continue;

            if (sprite_attributes[s].color_code == COLOR_TRANSPARENT)
                continue;
            
            if (((int) sprite_attributes[s].x >= min_x) && ((int) sprite_attributes[s].x <= max_x) &&
                ((int) sprite_attributes[s].y >= min_y) && ((int) sprite_attributes[s].y <= max_y))
            {
                sprite_collision = sprite_list[s];
                break;

            }

        }
    }

    return sprite_collision;
}
*/



int my_rand(void)
{
    int r = rand() % 10;

    return r;
}


void main()
{
    int  answer;
    bool steerable = false;
    SPRITE_STATE  hel_sprites[32];
    SPRITE_STATE  jet_sprites[32];
    SPRITE_STATE  man_sprites[32];
    SPRITE_STATE  shot_sprites[32];
    SPRITE_STATE  bomb_sprites[32];

    srand(1);
    mode_text();
    int right_start_x = 0;
    int left_start_x = 256 - 8 * 3;

    /******** INIT ******************/
    bool left = false;
    for(int i=0; i<32; i++)
    {
        hel_sprites[i].enable = false;
        hel_sprites[i].state = -1;
        hel_sprites[i].sprite = -1;
        hel_sprites[i].jumpers = 1;
        hel_sprites[i].going_left = left;
        hel_sprites[i].wait = my_rand();
        hel_sprites[i].y = SPRITE_TERMINATOR;

        jet_sprites[i].enable = false;
        jet_sprites[i].state = -1;
        jet_sprites[i].sprite = -1;
        jet_sprites[i].bomb = 1;
        jet_sprites[i].going_left = left;
        jet_sprites[i].wait = my_rand();
        jet_sprites[i].y = SPRITE_TERMINATOR;

        man_sprites[i].enable = false;
        man_sprites[i].state = -1;
        man_sprites[i].sprite = -1;
        man_sprites[i].y = SPRITE_TERMINATOR;

        shot_sprites[i].enable = false;
        shot_sprites[i].state = -1;
        shot_sprites[i].sprite = -1;
        shot_sprites[i].y = SPRITE_TERMINATOR;

        bomb_sprites[i].enable = false;
        bomb_sprites[i].state = -1;
        bomb_sprites[i].sprite = -1;
        bomb_sprites[i].y = SPRITE_TERMINATOR;

        left = ! left;
    }


    //answer = introduction(0);
    //steerable = ((answer == 'Y') || (answer == 'y'));

    //create_ground_and_turret();
    //getchar();
    //memset(sprite_attributes, 0, sizeof(SPRITE_ATTRIBUTE) * 32);

    mode_graphics_ii();

    //clr(' ');
    //test_char_color();


    //clr(' ');
    //test_sprites();
    //getchar();

    clr(' ');

    create_text_ground();
    create_text_turret();


    int helicopters = 2;
    int jets        = 2;
    int men         = 0;
    int shots       = 4;

    int emergency_stop = 0;

    // *************PREPARE THIS WAVE

    int hel_pos = 16*2;

    for(int i=0; i<helicopters; i++)
    {
        hel_sprites[i].enable = true;
        hel_sprites[i].state = -1;
        hel_sprites[i].jumpers = 1;
        
        if (hel_sprites[i].going_left)
            hel_sprites[i].x = left_start_x;
        else
            hel_sprites[i].x = right_start_x;

        hel_sprites[i].y = hel_pos;
        hel_sprites[i].wait = my_rand()*3;
        hel_pos += 16;

    }

    int jet_pos = 0;

    for (int i = 0; i < jets; i++)
    {
        jet_sprites[i].enable = true;
        jet_sprites[i].state = -1;
        if (jet_sprites[i].going_left)
            jet_sprites[i].x = left_start_x;
        else
            jet_sprites[i].x = right_start_x;

        jet_sprites[i].y = jet_pos;
        jet_sprites[i].wait = my_rand()*3;
        jet_pos += 16;
    }

    int direction = 1;
    int n;
    int start, stop, inc;
    for (;;)
    {
        for (int repeat=0; repeat<2; repeat++)
        {
            //direction = -direction;
            if (direction == 1)
            {
                start = 0;
                stop= 32;
                inc = 1;
            } else
            {
                start = 31;
                stop = -1;
                inc = -1;
            }
            int sprite = 0;
            int men = 0;

            for(int i=start; i != stop; i += inc)
            {
                if (hel_sprites[i].enable)
                {
                    if (hel_sprites[i].state != -1)
                    {
                        //sprintf(title, "About to put hel[%d] pre-sprite %d", i, sprite);
                        //debug(title,1);
                        n = helicopter(sprite, hel_sprites[i]);

                        sprite += n;
                        if (sprite >= 31)
                        {
                            sprite -= n;
                            break;
                        }
                        //sprintf(title, "hel[%d].state: %d sprites %d       ", i, hel_sprites[i].state, sprite);
                        //debug(title, 1);
                    } else
                    {
                        if (hel_sprites[i].wait > 0)
                        {
                            hel_sprites[i].wait--;
                        } else
                        {
                            hel_sprites[i].state = 0;
                            hel_sprites[i].wait = 0;
                        }

                        //sprintf(title, "hel[%d].state: %d wait %d  ", i, hel_sprites[i].state, hel_sprites[i].wait);
                        //debug(title, 1);
                    }
                }

                if (jet_sprites[i].enable)
                {
                    if (jet_sprites[i].state != -1)
                    {
                        //sprintf(title, "about to put jet[%d] pre-sprite %d", i, sprite);
                        //debug(title,2);
                        n = jet(sprite, jet_sprites[i]);

                        sprite += n;
                        if (sprite >= 31)
                        {
                            sprite -=n;
                            break;
                        }
                        //sprintf(title, "jet[%d].state: %d sprites %d       ", i, jet_sprites[i].state, sprite);
                        //debug(title, 2);
                    }
                    else
                    {
                        if (jet_sprites[i].wait > 0)
                        {
                            jet_sprites[i].wait--;
                        }
                        else
                        {
                            jet_sprites[i].state = 0;
                            jet_sprites[i].wait = 0;
                        }

                        //sprintf(title, "jet[%d].state: %d wait %d  ", i, jet_sprites[i].state, jet_sprites[i].wait);
                        //debug(title, 2);
                    }
                }

                if (emergency_stop)
                {
                    if (man_sprites[i].enable)
                    {
                        sprintf(title, "emergency stop[%d] man.state:%d (%d,%d)", i, man_sprites[i].state, man_sprites[i].x, man_sprites[i].y);
                        debug(title, 7);
                        getchar();
                        sprintf(title, "                     ");
                        debug(title,7);
                    }
                }

                if (man_sprites[i].enable)
                {
                    sprintf(title, "JUMPED (%d,%d) sprite %d", man_sprites[i].x, man_sprites[i].y, sprite);
                    debug(title, 7);
                    getchar();
                    n = parachuter(sprite, man_sprites[i]);
                    getchar();

                    sprite += n;
                    if (sprite >= 31)
                    {
                        sprite -= n;
                        break;
                    }

                    sprintf(title, "man[%d].state: %d (%d,%d))  ", i, man_sprites[i].state, man_sprites[i].x, man_sprites[i].y);
                    debug(title, 3);
                }


                if (shot_sprites[i].state != -1)
                {
                    // n = shots(&shot_sprites[i].sprite, shot_sprites[i].x, shot_sprites[i].y);
                    // sprites += n
                }

                if (bomb_sprites[i].state != -1)
                {
                    // n = bomb(&bomb_sprites[i].sprite, bomb_sprites[i].x, bomb_sprites[i].y);
                    // sprites += n
                }

            }

            if (sprite < 31)
                sprite_attributes[sprite+1].y = SPRITE_TERMINATOR;


            sprintf(title, "total sprites#:%d   ", sprite);
            debug(title, 20);
            //getchar();

            vwrite(sprite_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));
        } // for repeat
     /*
        // CHECK HELICOPTER FOR COLLISONS
        int sprite_collision;
        for (int i=0; i<32; i++)
        {
            if (hel_sprites[i].sprite == -1)
                break;
            sprite_collision = collision_detect(hel_sprites[i].sprite);
            if (sprite_collision != -1)
            {
                // dead
            }
        }

        // CHECK FOR JET COLLISIONS
        for (int i = 0; i < 32; i++)
        {
            if (jet_sprites[i].sprite == -1)
                break;

            sprite_collision = collision_detect(jet_sprites[i].sprite);
            if (sprite_collision != -1)
            {
                // dead
            }
        }
*/

        // SEE IF IT'S TIME TO DROP A MAN
        for (int i=0; i<32; i++)
        {
            if (hel_sprites[i].enable && (hel_sprites[i].jumpers > 0))
            {
                if (   (! hel_sprites[i].going_left && (hel_sprites[i].x > hel_sprites[i].drop_at))
                    || (  hel_sprites[i].going_left && (hel_sprites[i].x < hel_sprites[i].drop_at)))
                {   
                    hel_sprites[i].jumpers--;

                    man_sprites[men].enable = true;
                    man_sprites[men].x = hel_sprites[i].x;
                    man_sprites[men].x = hel_sprites[i].y+20;
                    man_sprites[men].state = JUMPED;
                    men++;

                    emergency_stop = 1;
                }
            }
        }



        /*

        for (int i=0; i<32; i++)
        {
            if (man_sprites[i].state != -1)
            {
                sprite_collision = collision_detect(man_sprites[i].state);
                if (sprite_collision != -1)
                {
                    man_sprites[i].state = FALLING;

                    for (int h=0; h<32; h++)
                    {
                        if (hel_sprites[h].sprite == sprite_collision)
                        {
                            hel_sprites[h].sprite = -1;
                            if (hel_sprites[h].going_left)
                                hel_sprites[h].x = left_start_x;
                            else
                                hel_sprites[h].x = right_start_x;
                        }

                        if (jet_sprites[h].sprite == sprite_collision)
                        {
                            jet_sprites[h].sprite = -1;
                            if (jet_sprites[h].going_left)
                                jet_sprites[h].x = left_start_x;
                            else
                                jet_sprites[h].x = right_start_x;
                        }
                    }
                    release_sprites(sprite_collision);
                }

            }
            
        }
        */

        // MOVE THE MEN
        for(int i=0; i<32; i++)
        {
            if (! man_sprites[i].enable)
                continue;

            if (man_sprites[i].state == FLOATING)
                man_sprites[i].y += 1;
            else
                man_sprites[i].y += 5;

            if (man_sprites[i].state <= JUMPED3)
                man_sprites[i].state++;

            if (man_sprites[i].y > 192 - 16)
            {
                man_sprites[i].enable = false;
                man_sprites[i].state = -1;
                int char_x;
                int char_y;
                man_sprites[i].y += 8;

                // replace the man sprite with a man character
                char_x = man_sprites[i].x / 8;
                char_y = man_sprites[i].y / 8;
                //vpoke(VRAM_NAME_TABLE + char_y * total_columns + char_x + 1, CHAR_MAN);
            }
        }

        // MOVE THE HELICOPTERS AND JETS
        for (int i = 0; i < 32; i++)
        {
            if (hel_sprites[i].state != -1)
            {
                if (hel_sprites[i].going_left)
                    hel_sprites[i].x -= 2;
                else
                    hel_sprites[i].x += 2;
            }

            if (jet_sprites[i].state != -1)
            {
                if (jet_sprites[i].going_left)
                    jet_sprites[i].x -= 2;
                else
                    jet_sprites[i].x += 2;
            }
        }

        /*
        if (bomb_state != -1)
        {
            bomb(&bomb_sprite, bomb_x, bomb_y);

            if (bomb_y >= 192)
                release_sprites();
            else
                bomb_y += 5;
        }
        */

        delay(50);
    }    

    end();
    
}
