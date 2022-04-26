#ifndef _SPRITESET_H
#define _SPRITESET_H


// pattern position
#define CHUTE_LEFT           0
#define CHUTE_RIGHT          1
#define CHUTE_MAN            2
#define GROUND_MAN           3
#define CHUTE_LEFT_FAIL      4
#define CHUTE_RIGHT_FAIL     5
#define CHUTE_LEFT_FAIL2     6
#define CHUTE_RIGHT_FAIL2    7
#define CHOPTER_BLADE_LEFT   8
#define CHOPTER_BLADE_LEFT_MID 9
#define CHOPTER_BLADE_RIGHT_MID 10
#define CHOPTER_BLADE_RIGHT 11

#define CHOPTER_ROTOR_UP    12
#define CHOPTER_ROTOR_SIDE  13
#define CHOPTER_MID         14
#define CHOPTER_FRONT       15

#define LCHOPTER_ROTOR_UP   16
#define LCHOPTER_ROTOR_SIDE 17
#define LCHOPTER_MID        18
#define LCHOPTER_FRONT      19

#define SHOT                20

#define JET_END             21
#define JET_MID             22
#define JET_FRONT           23

#define LJET_FRONT          24
#define LJET_MID            25
#define LJET_END            26

#define BOMB                27
#define EXPLODE             28

#define SPRITE_TERMINATOR   208

extern unsigned char sprites[32 * 8];


typedef struct
{
    unsigned char y;
    unsigned char x;
    unsigned char sprite_pattern;
    unsigned char color_code : 4;
    unsigned char reserved : 3;    // always zero
    unsigned char early_clock : 1; // 1=shift left horizontally 32 pixels

} SPRITE_ATTRIBUTE;

typedef struct
{
    int enable;
    int sprite;
    int state;
    int x;
    int y;
    int drop_at;
    int going_left;
    int wait;
    int jumpers;
    int bomb;

} SPRITE_STATE;

extern SPRITE_ATTRIBUTE sprite_attributes[32];

#endif
