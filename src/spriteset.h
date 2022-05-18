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
#define CHOPTER_BLADE_RIGHT_MID 9
#define CHOPTER_BLADE_LEFT_MID 10
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

#define SPRITE_TYPE_NONE       0
#define SPRITE_TYPE_SHOT       1
#define SPRITE_TYPE_CHUTE      2
#define SPRITE_TYPE_MAN        3
#define SPRITE_TYPE_HELICOPTER 4
#define SPRITE_TYPE_JET        5
#define SPRITE_TYPE_BOMB       6
#define SPRITE_TYPE_EXPLODE    7
 
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
    int sprite_num; 
    int state;
    int flip;
    int inc_x;
    int inc_y;
    int x;
    int y;
    int x2; // used for collision detection
    int y2;
    int bomb_wait;
    int going_left;
    int jump_wait;
    int appearance_wait;
    int disappearance_wait;
    int jumpers;
    int bombs;
    int sprite_type;

} SPRITE_STATE;

extern SPRITE_ATTRIBUTE sprite_attributes[32];
extern SPRITE_ATTRIBUTE reverse_attributes[32];

#endif
