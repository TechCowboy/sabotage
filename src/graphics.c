#include <msx.h>
#include <msx/gfx.h>
#include <eos.h>
#include "src/graphics.h"
#include "src/colorset.h"
#include "src/spriteset.h"
#include "src/vdp_registers.h"
#include "src/charset.h"

int total_columns, total_rows;


void clr(unsigned char fillchar)
{
    fill(VRAM_NAME_TABLE, fillchar, total_rows * total_columns);
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

    r1.Blank_disable = 0;  // don't display anything until everything is set
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

    eos_write_vdp_register(1, r1.reg1);

    eos_write_vdp_register(0, r0.reg0);
    
    eos_write_vdp_register(2, r2.reg2);
    eos_write_vdp_register(4, r4.reg4);
    eos_write_vdp_register(7, r7.reg7);

    vwrite(character_set, VRAM_GENERATOR_TABLE, 256 * 8);

    r1.Blank_disable = 1; // display our new settings

    eos_write_vdp_register(1, r1.reg1);

    total_columns = 40;
    total_rows = 24;
}

/*

void mode_text(void)
{
    int r0, r1, r2, r3, r4, r5, r6, r7;

    r0 = 0x00;
    r1 = MASK_R1_16K | MASK_R1_BLANK | MASK_R1_INTERRUPT | MASK_R1_M1;

    r2 = VRAM_NAME_TABLE / 0x400;
    r4 = VRAM_GENERATOR_TABLE / 0x800;
    r7 = COLOR_WHITE << 4 | COLOR_BLACK;

    vwrite(character_set, VRAM_GENERATOR_TABLE, 256 * 8);

    eos_write_vdp_register(0, r0);
    eos_write_vdp_register(1, r1);
    eos_write_vdp_register(2, r2);
    eos_write_vdp_register(4, r4);
    eos_write_vdp_register(7, r7);

    total_columns = 40;
    total_rows = 24;
}
*/
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

    r0.M3 = 1;

    r1.M1 = 0;
    r1.M2 = 0;
    r1.RAM_16K = 1;
    r1.Blank_disable = 0; // don't display anything until everything is set
    r1.Interrupt_enable = 1;
    r1.Sprite_16x16_enable = 0;
    r1.Magnify_sprite = 0;

    r2.Base_address_of_name_table             = VRAM_NAME_TABLE / 0x400;
    r3.Base_address_of_color_table            = VRAM_COLOR_TABLE / 0x040;
    r4.Base_address_of_generator_table        = VRAM_GENERATOR_TABLE / 0x800;
    r5.Base_address_of_sprite_attribute_table = VRAM_SPRITE_ATTRIBUTES / 0x080;
    r6.Base_address_of_sprite_generator_table = VRAM_SPRITE_GENERATOR_TABLE / 0x800;

    r7.foreground_color = COLOR_LIGHT_RED;
    r7.background_color = COLOR_BLACK;

    eos_write_vdp_register(1, r1.reg1); // blank display

    vwrite(character_set, VRAM_GENERATOR_TABLE, 256 * 8);
    vwrite(character_set, VRAM_GENERATOR_TABLE, 256 * 8 * 2);
    vwrite(character_set, VRAM_GENERATOR_TABLE, 256 * 8 * 3);

    // top 8 rows
    memset(mode_ii_color_set,           SHIFTED_COLOR_DARK_GREEN   | COLOR_CYAN, 256 * 8);
    // mid 8 rows
    memset(mode_ii_color_set + 256 * 8, SHIFTED_COLOR_LIGHT_GREEN  | COLOR_LIGHT_YELLOW, 256 * 8);
    // bottom 8 rows
    memset(mode_ii_color_set + 512 * 8, SHIFTED_COLOR_LIGHT_YELLOW | COLOR_LIGHT_RED, 256 * 8);

    vwrite(mode_ii_color_set, VRAM_COLOR_TABLE, 3 * 256 * 8);

    vwrite(sprite_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));

    vwrite(sprites, VRAM_SPRITE_GENERATOR_TABLE, 32 * 8);

    r1.Blank_disable = 1; // show display

    eos_write_vdp_register(0, r0.reg0);

    eos_write_vdp_register(2, r2.reg2);
    eos_write_vdp_register(3, r3.reg3);
    eos_write_vdp_register(4, r4.reg4);
    eos_write_vdp_register(5, r5.reg5);
    eos_write_vdp_register(6, r6.reg6);
    eos_write_vdp_register(7, r7.reg7);

    eos_write_vdp_register(1, r1.reg1);
    
    total_columns = 32;
    total_rows = 24;
}
/*
void mode_graphics_ii(void)
{
    int r0, r1, r2, r3, r4, r5, r6, r7;

    r0 = MASK_R0_M3;                                      // M3=1
    r1 = MASK_R1_16K | MASK_R1_BLANK | MASK_R1_INTERRUPT; // M1=0 M2=0

    r2 = VRAM_NAME_TABLE / 0x400;
    r3 = VRAM_COLOR_TABLE / 0x040;
    r4 = VRAM_GENERATOR_TABLE / 0x800;
    r5 = VRAM_SPRITE_ATTRIBUTES / 0x080;
    r6 = VRAM_SPRITE_GENERATOR_TABLE / 0x800;
    r7 = SHIFTED_COLOR_WHITE | COLOR_BLACK;

    vwrite(character_set, VRAM_GENERATOR_TABLE, 256 * 8);

    // set_color_region(mode_ii_color_set +    0, 2048, SHIFTED_COLOR_CYAN | COLOR_LIGHT_YELLOW);
    // set_color_region(mode_ii_color_set + 2048, 2048, SHIFTED_COLOR_CYAN | COLOR_LIGHT_YELLOW);
    // set_color_region(mode_ii_color_set + 4096, 2048, SHIFTED_COLOR_CYAN | COLOR_LIGHT_YELLOW);

    memset(mode_ii_color_set, SHIFTED_COLOR_WHITE | COLOR_CYAN, 6144);
    vwrite(mode_ii_color_set, VRAM_COLOR_TABLE, 6144);

    vwrite(sprite_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));

    vwrite(sprites, VRAM_SPRITE_GENERATOR_TABLE, 32 * 8);

    eos_write_vdp_register(0, r0);
    eos_write_vdp_register(1, r1);
    eos_write_vdp_register(2, r2);
    eos_write_vdp_register(3, r3);
    eos_write_vdp_register(4, r4);
    eos_write_vdp_register(5, r5);
    eos_write_vdp_register(6, r6);
    eos_write_vdp_register(7, r7);

    total_columns = 32;
    total_rows = 24;
}
*/