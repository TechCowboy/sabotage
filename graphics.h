#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#define VRAM_GENERATOR_TABLE 0x0000        // Library of all the patterns (Max Length is Graphics II: 6144)
#define VRAM_NAME_TABLE 0x1800             // Contains entries which correspond to the screen display (Length 768)
#define VRAM_COLOR_TABLE 0x2000            // Color corresponding to the entries in the name table Length (Max Length is 6144)
#define VRAM_SPRITE_GENERATOR_TABLE 0x3800 // Library of all sprite patterns (Length 256)
#define VRAM_SPRITE_TABLE 0x3c00           // (Length 256)
#define VRAM_SPRITE_ATTRIBUTES 0x3d00      // (Length 128)

void eos_write_vdp_register(unsigned char reg, unsigned char val);
unsigned char eos_read_vdp_register(void);

void clr(unsigned char c);
void mode_text(void);
void mode_text2(void);
void mode_graphics_ii(void);
void mode_graphics_ii2(void);

extern int total_columns;
extern int total_rows;

#endif
