#include <string.h>
#include <eos.h>
#include <msx.h>
#include <msx/gfx.h>
#include <arch/z80.h>
#include "graphics.h"
#include "colorset.h" 
#include "charset.h"
#include "misc.h"
#include "vdp_registers.h"

extern char title[];

char gun[7][2] =
    {
        {CHAR_TURRET_LEFT_GUN_0, CHAR_TURRET_RIGHT}, // 0
        {CHAR_TURRET_LEFT_GUN_1, CHAR_TURRET_RIGHT}, // 1
        {CHAR_TURRET_LEFT_GUN_2, CHAR_TURRET_RIGHT}, // 2
        {CHAR_TURRET_LEFT_GUN_3, CHAR_TURRET_RIGHT_GUN_3},  // 3
        {CHAR_TURRET_LEFT, CHAR_TURRET_RIGHT_GUN_4}, // 4
        {CHAR_TURRET_LEFT, CHAR_TURRET_RIGHT_GUN_5}, // 5
        {CHAR_TURRET_LEFT, CHAR_TURRET_RIGHT_GUN_6}  // 6

};

int mygetchar(void)
{
    return getchar()  ;
}

int introduction(void)
{
    int key;
    int random=0;
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
    }
    vwrite(intro, VRAM_NAME_TABLE, MIN(length, total_columns * total_rows));

    //do
    //{
    //    key = eos_end_read_keyboard();
    //    eos_start_read_keyboard();
    //    random++;
    //} while (key <= 1);

    mygetchar();
    
    srand(random);

    return key;
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

    rotation = rotation & (~ 16384);
    rotation = MIN(6, rotation);
    rotation = MAX(0, rotation);
    
    for (y = total_rows - 1; y < total_rows + 1; y++)
    {
        x = total_columns / 2 - 1;
        if (y == total_columns - 1)
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_LEFT);
        else
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);
        
        if (y == total_columns - 1)
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_RIGHT);
        else
            vpoke(VRAM_NAME_TABLE + y * total_columns + x++, CHAR_STAND_MID);
    }

    y = total_rows - 2;
    x = total_columns / 2 - 1;

    for(int i=0; i<2; i++)
    {
        vpoke(VRAM_NAME_TABLE + y * total_columns + x++, gun[rotation][i]);
    }

}

void end_game()
{
    mode_text();
    clr(' ');
    sprintf(title, "The End.");
    vprint(title,0);
}

void test_char_color(void)
{
    TMS_REGISTER_3 r3;
    char message[] = {"12345678901234567890123456789012"};

    r3.Base_address_of_color_table = VRAM_COLOR_TABLE / 0x040;
    set_vdp(3,r3.value);

    unsigned short addr = mode_ii_color_set;
    int length = 8 * 256;
    int size = 0;

    memset(addr, SHIFTED_COLOR_WHITE     | COLOR_BLACK, length);
    size += length;
    addr += length;

    memset(addr, SHIFTED_COLOR_LIGHT_YELLOW | COLOR_BLACK,      length);
    size += length;
    addr += length;

    memset(addr, SHIFTED_COLOR_LIGHT_YELLOW | COLOR_GRAY,       length);
    size += length;
    addr += length;

    eos_write_vram(size, VRAM_COLOR_TABLE, mode_ii_color_set);

    for(int line=0; line<23; line++)
        vprint(message, line);

    mygetchar();

}
