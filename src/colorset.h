// TMS99xxA colours
#ifndef _COLOR_SET
#define _COLOR_SET

#include <stdio.h>
#include <string.h>


#define COLOR_TRANSPARENT  0x00
#define COLOR_BLACK        0x01
#define COLOR_MEDIUM_GREEN 0x02
#define COLOR_LIGHT_GREEN  0x03
#define COLOR_DARK_BLUE    0x04
#define COLOR_LIGHT_BLUE   0x05
#define COLOR_DARK_RED     0x06
#define COLOR_CYAN         0x07
#define COLOR_MEDIUM_RED   0x08
#define COLOR_LIGHT_RED    0x09
#define COLOR_DARK_YELLOW  0x0a
#define COLOR_LIGHT_YELLOW 0x0b
#define COLOR_DARK_GREEN   0x0c
#define COLOR_MAGENTA      0x0d
#define COLOR_GRAY         0x0e
#define COLOR_WHITE        0x0f

#define SHIFTED_COLOR_TRANSPARENT  0x00
#define SHIFTED_COLOR_BLACK        (COLOR_BLACK << 4)
#define SHIFTED_COLOR_MEDIUM_GREEN (COLOR_MEDIUM_GREEN << 4)
#define SHIFTED_COLOR_LIGHT_GREEN  (COLOR_LIGHT_GREEN << 4)
#define SHIFTED_COLOR_DARK_BLUE    (COLOR_DARK_BLUE << 4)
#define SHIFTED_COLOR_LIGHT_BLUE   (COLOR_LIGHT_BLUE << 4)
#define SHIFTED_COLOR_DARK_RED     (COLOR_DARK_RED << 4)
#define SHIFTED_COLOR_CYAN         (COLOR_CYAN << 4)
#define SHIFTED_COLOR_MEDIUM_RED   (COLOR_MEDIUM_RED << 4)
#define SHIFTED_COLOR_LIGHT_RED    (COLOR_LIGHT_RED << 4)
#define SHIFTED_COLOR_DARK_YELLOW  (COLOR_DARK_YELLOW << 4)
#define SHIFTED_COLOR_LIGHT_YELLOW (COLOR_LIGHT_YELLOW << 4)
#define SHIFTED_COLOR_DARK_GREEN   (COLOR_DARK_GREEN << 4)
#define SHIFTED_COLOR_MAGENTA      (COLOR_MAGENTA << 4)
#define SHIFTED_COLOR_GRAY         (COLOR_GRAY << 4)
#define SHIFTED_COLOR_WHITE        (COLOR_WHITE << 4)

extern unsigned char mode_ii_color_set[2048 * 3];
void set_color_region(char *addr, unsigned char pattern, int length);

#endif
