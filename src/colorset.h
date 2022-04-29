// TMS99xxA colours
#ifndef _COLOR_SET
#define _COLOR_SET

#include <stdio.h>
#include <string.h>

#define CHAR_TURRET_LEFT  128
#define CHAR_TURRET_RIGHT 129
#define CHAR_STAND_LEFT   130
#define CHAR_STAND_MID    131
#define CHAR_STAND_RIGHT  132
#define CHAR_GROUND       135

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
#define SHIFTED_COLOR_BLACK        0x10
#define SHIFTED_COLOR_MEDIUM_GREEN 0x20
#define SHIFTED_COLOR_LIGHT_GREEN  0x30
#define SHIFTED_COLOR_DARK_BLUE    0x40
#define SHIFTED_COLOR_LIGHT_BLUE   0x50
#define SHIFTED_COLOR_DARK_RED     0x60
#define SHIFTED_COLOR_CYAN         0x70
#define SHIFTED_COLOR_MEDIUM_RED   0x80
#define SHIFTED_COLOR_LIGHT_RED    0x90
#define SHIFTED_COLOR_DARK_YELLOW  0xa0
#define SHIFTED_COLOR_LIGHT_YELLOW 0xb0
#define SHIFTED_COLOR_DARK_GREEN   0xc0
#define SHIFTED_COLOR_MAGENTA      0xd0
#define SHIFTED_COLOR_GRAY         0xe0
#define SHIFTED_COLOR_WHITE        0xf0

extern unsigned char mode_ii_color_set[2048 * 3];

#endif
