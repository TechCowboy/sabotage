#include <msx.h>
#include <msx/gfx.h>
#include <eos.h>
#include "graphics.h"
#include "colorset.h"
#include "spriteset.h"
#include "vdp_registers.h"
#include "charset.h"

int total_columns, total_rows;
extern unsigned char character_colour[];

void clr(unsigned char fillchar)
{
    fillchar = fillchar - 32;
    fill(VRAM_NAME_TABLE, fillchar, total_rows * total_columns);
    
}

void vprint(char *message, int line)
{
    char lmessage[960];
    strcpy(lmessage, message);
    if (strlen(lmessage) == 0)
        strcat(lmessage, " ");

    int length = strlen(lmessage);

    for (int i = 0; i < length; i++)
        lmessage[i] -= 32;

    //vwrite(lmessage, VRAM_NAME_TABLE + line * total_columns, length);
    eos_write_vram(length, VRAM_NAME_TABLE + line * total_columns, lmessage);
    // mygetchar();
}

void mode_text(void)
{
    TMS_REGISTER_0 r0;
    TMS_REGISTER_1 r1;
    TMS_REGISTER_2 r2;
    TMS_REGISTER_4 r4;
    TMS_REGISTER_7 r7;



    r0.enable_VDP_input = 0;
    r0.M3 = 0;

    r1.Show_screen = 0; // don't display anything until everything is set
    r1.Interrupt_enable = 1;
    r1.M1 = 1;
    r1.M2 = 0;
    r1.Magnify_sprite = 0;
    r1.RAM_16K = 1;
    r1.Sprite_16x16_enable = 0;
    

    r2.Base_address_of_name_table = VRAM_NAME_TABLE / 0x400;

    r4.Base_address_of_generator_table = VRAM_GENERATOR_TABLE / 0x800;

    r7.background_color = COLOR_BLACK;
    r7.foreground_color = COLOR_WHITE;

    eos_write_vdp_register(1, r1.value);

    eos_write_vdp_register(0, r0.value);

    eos_write_vdp_register(2, r2.value);
    eos_write_vdp_register(4, r4.value);
    eos_write_vdp_register(7, r7.value);

    eos_write_vram(256 * 8, VRAM_GENERATOR_TABLE, character_set);

    r1.Show_screen = 1; // display our new settings
    eos_write_vdp_register(1, r1.value);

    total_columns = 40;
    total_rows = 24;
}

void mode_graphics_i(void)
{
    TMS_REGISTER_0 r0;
    TMS_REGISTER_1 r1;
    TMS_REGISTER_2 r2;
    TMS_REGISTER_3 r3;
    TMS_REGISTER_4 r4;
    TMS_REGISTER_5 r5;
    TMS_REGISTER_6 r6;
    TMS_REGISTER_7 r7;

    /* MEMORY MAP

       VRAM_GENERATOR_TABLE
       0000 - 07FF - char set

       VRAM_COLOR_TABLE
       2000 - 07FF - color set

       VRAM_SPRITE_GENERATOR_TABLE
       3800 - 38FF - sprite table

       VRAM_SPRITE_ATTRIBUTES
       3D00 - 3D7F - sprite attributes
    */

    // Graphics mode 1  M1=0, M2=0, M3=0
    r0.M3 = 0;
    r0.enable_VDP_input = 0;

    r1.M1 = 0;
    r1.M2 = 0;
    r1.RAM_16K = 1;     // This is a 16K graphics chip
    r1.Show_screen = 0; // don't display anything until everything is set
    r1.Interrupt_enable = 0;
    r1.Sprite_16x16_enable = 0;
    r1.Magnify_sprite = 0;

    eos_write_vdp_register(1, r1.value); // blank the display

    r2.Base_address_of_name_table = VRAM_NAME_TABLE / 0x400;
    r3.Base_address_of_color_table = VRAM_COLOR_TABLE / 0x040;
    r4.Base_address_of_generator_table = VRAM_GENERATOR_TABLE / 0x800;
    r5.Base_address_of_sprite_attribute_table = VRAM_SPRITE_ATTRIBUTES / 0x080;
    r6.Base_address_of_sprite_generator_table = VRAM_SPRITE_GENERATOR_TABLE / 0x800;

    // r7.foreground_color = COLOR_LIGHT_RED;
    // r7.background_color = COLOR_BLACK;

    int addr = VRAM_GENERATOR_TABLE;
    int length = 256 * 8;
    eos_write_vram(length, addr, character_set);

    // top 8 rows
    memset(mode_ii_color_set, SHIFTED_COLOR_DARK_GREEN | COLOR_CYAN, length);

    eos_write_vram(length, VRAM_COLOR_TABLE, mode_ii_color_set);

    eos_write_vram(32 * sizeof(SPRITE_ATTRIBUTE), VRAM_SPRITE_ATTRIBUTES, sprite_attributes);

    eos_write_vram(32 * 8, VRAM_SPRITE_GENERATOR_TABLE, sprites);

    eos_write_vdp_register(2, r2.value);
    eos_write_vdp_register(3, r3.value);
    eos_write_vdp_register(4, r4.value);
    eos_write_vdp_register(5, r5.value);
    eos_write_vdp_register(6, r6.value);
    // eos_write_vdp_register(7, r7.reg7);

    r1.Show_screen = 1; // show display
    eos_write_vdp_register(1, r1.value);

    total_columns = 32;
    total_rows = 24;
}

/*
color mode ii

The first character is the top-left one, the second the one to the right of
it. Character no. 32 the first character of the second row. There are three
colour and three pattern tables. The character number is as follows:

The first  8 rows: byte from PN + 000h
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
void mode_graphics_ii(void)
{
    TMS_REGISTER_0 r0;
    TMS_REGISTER_1 r1;
    TMS_REGISTER_2 r2;
    TMS_REGISTER_3 r3;
    TMS_REGISTER_4 r4;
    TMS_REGISTER_5 r5;
    TMS_REGISTER_6 r6;
    TMS_REGISTER_7 r7;

    /* MEMORY MAP

       VRAM_GENERATOR_TABLE
       0000 - 07FF - char set top
       0800 - 0FFF - char set mid
       1000 - 17FF - char set bottom

       VRAM_COLOR_TABLE
       2000 - 07FF - color set top
       2800 - 2FFF - color set mid
       3000 - 37FF - color set bottom

       VRAM_SPRITE_GENERATOR_TABLE
       3800 - 38FF - sprite table

       VRAM_SPRITE_ATTRIBUTES
       3D00 - 3D7F - sprite attributes
    */

    // Graphics mode 2  M1=0, M2=0, M3=1
    r0.M3                  = 1;
    r0.enable_VDP_input    = 0; 

    r1.M1                  = 0;
    r1.M2                  = 0;
    r1.RAM_16K             = 1;     // This is a 16K graphics chip
    r1.Show_screen         = 0;     // don't display anything until everything is set
    r1.Interrupt_enable    = 1;     //
    r1.Sprite_16x16_enable = 0;     // Sprites are only 8x8
    r1.Magnify_sprite      = 0;     // No need to make them twice normal size

    eos_write_vdp_register(1, r1.value); // blank the display while we set up registers

    r2.Base_address_of_name_table             = VRAM_NAME_TABLE / 0x400;
    r3.Base_address_of_color_table            = VRAM_COLOR_TABLE / 0x040;
    r4.Base_address_of_generator_table        = VRAM_GENERATOR_TABLE / 0x800;
    r5.Base_address_of_sprite_attribute_table = VRAM_SPRITE_ATTRIBUTES / 0x080;
    r6.Base_address_of_sprite_generator_table = VRAM_SPRITE_GENERATOR_TABLE / 0x800;

    //r7.foreground_color = COLOR_LIGHT_RED;
    //r7.background_color = COLOR_BLACK;

    

    int addr = VRAM_GENERATOR_TABLE; 
    int length = 256*8;
    eos_write_vram(length, addr, character_set);
    addr += length;    
    eos_write_vram(length, addr, character_set);
    addr += length;
    eos_write_vram(length, addr, character_set);

    // top 8 rows
    memset(mode_ii_color_set,           SHIFTED_COLOR_DARK_GREEN | COLOR_CYAN, 256 * 8);
    // mid 8 rows
    memset(mode_ii_color_set + 256 * 8, SHIFTED_COLOR_LIGHT_GREEN | COLOR_LIGHT_YELLOW, 256 * 8);
    // bottom 8 rows
    memset(mode_ii_color_set + 512 * 8, SHIFTED_COLOR_LIGHT_YELLOW | COLOR_LIGHT_RED, 256 * 8);

    eos_write_vram(3 * 256 * 8, VRAM_COLOR_TABLE, mode_ii_color_set);

    eos_write_vram(32 * sizeof(SPRITE_ATTRIBUTE), VRAM_SPRITE_ATTRIBUTES, sprite_attributes);

    eos_write_vram(32 * 8, VRAM_SPRITE_GENERATOR_TABLE, sprites);

    eos_write_vdp_register(2, r2.value);
    eos_write_vdp_register(3, r3.value);
    eos_write_vdp_register(4, r4.value);
    eos_write_vdp_register(5, r5.value);
    eos_write_vdp_register(6, r6.value);
    //eos_write_vdp_register(7, r7.reg7);

    r1.Show_screen = 1; // show display
    eos_write_vdp_register(1, r1.value);

    total_columns = 32;
    total_rows = 24;

    //char temp[80];
    //sprintf(temp, "%02x ", r1.reg1);
    //vprintf(temp, 0);
    //getchar();
}
