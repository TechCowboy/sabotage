#ifndef _SPRITECTRL_H
#define _SPRITECTRL_H

#define OFF_SCREEN -1
#define ON_SCREEN 0

#define OFF_SCREEN_Y 200
#define BOTTOM_SCREEN_Y 192

#define NO_SPRITE -1

#define JUMPED 0
#define JUMPED1 1
#define JUMPED2 2
#define JUMPED3 3
#define FLOATING 4
#define FALLING 5
#define GROUNDED 6

#define DROPPED 0

#define FLOATING_SPEED 1
#define FALLING_SPEED 5
#define HELICOPTER_SPEED 1
#define JET_SPEED 4
#define SHOT_X_SPEED 1
#define SHOT_Y_SPEED 1
#define BOMB_X_SPEED 1
#define BOMB_Y_SPEED 4

int set_y(int y);
int parachuter(int sprite_number, SPRITE_STATE info);
int jet(int sprite_number, SPRITE_STATE info);
int bomb(int sprite_number, SPRITE_STATE info);
int shot(int sprite_number, SPRITE_STATE info);
int helicopter(int sprite_number, SPRITE_STATE info);

#endif

