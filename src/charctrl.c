#include <string.h>
#include <eos.h>
#include <msx.h>
#include <msx/gfx.h>
#include <arch/z80.h>
#include "graphics.h"
#include "colorset.h" 
#include "charset.h"
#include "misc.h"

extern char title[];

char gun[5][4] =
    {
        {CHAR_LEFT_GUN_0, CHAR_TURRET_LEFT_GUN_0, CHAR_TURRET_RIGHT, CHAR_BLANK},
        {CHAR_LEFT_GUN_1, CHAR_TURRET_LEFT_GUN_1, CHAR_TURRET_RIGHT, CHAR_BLANK},
        {CHAR_LEFT_GUN_2, CHAR_TURRET_LEFT_GUN_2, CHAR_TURRET_RIGHT, CHAR_BLANK},
        {CHAR_LEFT_GUN_3, CHAR_TURRET_LEFT_GUN_3, CHAR_TURRET_RIGHT, CHAR_BLANK},
        {CHAR_LEFT_GUN_4, CHAR_TURRET_LEFT_GUN_4, CHAR_TURRET_RIGHT, CHAR_BLANK}

};

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

    int length = strlen(intro);
    for (int x = 0; x < length; x++)
    {
        intro[x] -= 32;
        intro[x] = intro[x] + 256 * group;
    }
    vwrite(intro, VRAM_NAME_TABLE, MIN(length, total_columns * total_rows));

    return getchar();
}

void create_text_ground(void)
{

    int y = total_rows - 1;
    int x;

    for (x = 0; x < total_columns; x++)
        vpoke(VRAM_NAME_TABLE + y * total_columns + x, CHAR_GROUND);
}

void create_text_turret(int rotation)
{

    int y = total_rows - 1;
    int x;

    for (y = total_rows - 2; y < total_rows + 1; y++)
    {
        x = total_columns / 2 - 2;
        if (y == total_columns - 1)
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_LEFT);
        else
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);

        vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);
        vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);
        
        if (y == total_columns - 1)
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_RIGHT);
        else
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);
    }

    y = total_rows - 3;
    x = total_columns / 2 - 2;

    for(int i=0; i<4; i++)
    {
        vpoke(VRAM_NAME_TABLE + y * total_columns + x++, gun[rotation][i]);
    }
}

void end()
{
    mode_text();
    clr(' ');
    sprintf(title, "The End.");
    vprint(title,0);
}

void test_char_color(void)
{
    int x;
    int y = 0;

    int color1 = SHIFTED_COLOR_CYAN | COLOR_DARK_RED;
    int color2 = SHIFTED_COLOR_DARK_RED | COLOR_CYAN;
    int color3 = SHIFTED_COLOR_DARK_YELLOW | COLOR_LIGHT_BLUE;
    vpoke(VRAM_COLOR_TABLE, color1);
    vpoke(VRAM_COLOR_TABLE + 2048, color2);
    vpoke(VRAM_COLOR_TABLE + 4096, color3);

    for (int z = 0; z < 4096; z += 2048)
    {
        for (y = 0; y < total_rows; y += 2)
        {
            for (x = 0; x < total_columns; x += 2)
            {
                vpoke(VRAM_NAME_TABLE + y * total_columns + x, z);
            }
        }
    }
}
