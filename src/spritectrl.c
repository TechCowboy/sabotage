#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msx.h>
#include <msx/gfx.h>
#include <arch/z80.h>
#include "spriteset.h"
#include "spritectrl.h"
#include "colorset.h"
#include "charctrl.h"

static char temp[32];

int set_y(int y)
{
    if (y == SPRITE_TERMINATOR)
        y++;

    return y;
}

int parachuter(int sprite_number, SPRITE_STATE *info)
{
    info->sprite_num = sprite_number;

    if (info->state <= JUMPED3)
    {
        // hide chutes
        sprite_attributes[sprite_number].x = info->x;
        sprite_attributes[sprite_number].y = set_y(OFF_SCREEN_Y);

        sprite_attributes[sprite_number + 1].x = info->x + 8;
        sprite_attributes[sprite_number + 1].y = set_y(OFF_SCREEN_Y);
    }
    else
    {
        // parachute positions
        sprite_attributes[sprite_number].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number].x = info->x;
        sprite_attributes[sprite_number].y = set_y(info->y);

        sprite_attributes[sprite_number + 1].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 1].x = info->x + 8;
        sprite_attributes[sprite_number + 1].y = set_y(info->y);

        // normal parachute
        if (info->state == FLOATING)
        {
            sprite_attributes[sprite_number].sprite_pattern = CHUTE_LEFT;

            sprite_attributes[sprite_number + 1].sprite_pattern = CHUTE_RIGHT;
        }

        // broken parachute
        if (info->state == FALLING)
        {
            sprite_attributes[sprite_number].sprite_pattern = info->flip ? CHUTE_LEFT_FAIL : CHUTE_LEFT_FAIL2;

            sprite_attributes[sprite_number + 1].sprite_pattern = info->flip ? CHUTE_RIGHT_FAIL : CHUTE_RIGHT_FAIL2;
        }
    }

    // man position
    sprite_attributes[sprite_number + 2].color_code = COLOR_DARK_BLUE;
    sprite_attributes[sprite_number + 2].x = info->x + 4;
    sprite_attributes[sprite_number + 2].y = set_y(info->y + 8);

    sprite_attributes[sprite_number + 2].sprite_pattern = (info->state == FLOATING) ? CHUTE_MAN : GROUND_MAN;

    info->x2 = info->x + 4 + 8;
    info->y2 = info->y + 8 + 8;
    
    return 3; // 3 sprites used
}

int jet(int sprite_number, SPRITE_STATE *info)
{
    static bool fail_sprite_flag = true;

    info->sprite_num = sprite_number;

    sprite_attributes[sprite_number].sprite_pattern = info->going_left ? LJET_FRONT : JET_END;

    sprite_attributes[sprite_number].color_code = COLOR_BLACK;

    sprite_attributes[sprite_number].x = info->x;
    sprite_attributes[sprite_number].y = set_y(info->y);

    sprite_attributes[sprite_number + 1].sprite_pattern = info->going_left ? LJET_MID : JET_MID;
    sprite_attributes[sprite_number + 1].color_code = COLOR_BLACK;

    sprite_attributes[sprite_number + 1].x = info->x + 8;
    sprite_attributes[sprite_number + 1].y = set_y(info->y);

    sprite_attributes[sprite_number + 2].sprite_pattern = info->going_left ? LJET_END : JET_FRONT;
    sprite_attributes[sprite_number + 2].color_code = COLOR_BLACK;

    sprite_attributes[sprite_number + 2].x = info->x + 16;
    sprite_attributes[sprite_number + 2].y = set_y(info->y);

    info->x2 = info->x + 16 +8;
    info->y2 = info->y + 8;

    return 3; // 3 sprites used
}

int bomb(int sprite_number, SPRITE_STATE *info)
{
    info->sprite_num = sprite_number;

    sprite_attributes[sprite_number].sprite_pattern = BOMB;
    sprite_attributes[sprite_number].color_code = COLOR_LIGHT_RED;
    sprite_attributes[sprite_number].x = info->x;
    sprite_attributes[sprite_number].y = set_y(info->y);

    info->x2 = info->x + 8;
    info->y2 = info->y + 8;

    return 1;
}

int shot(int sprite_number, SPRITE_STATE *info)
{
    info->sprite_num = sprite_number;

    sprite_attributes[sprite_number].sprite_pattern = SHOT;
    sprite_attributes[sprite_number].color_code = COLOR_MAGENTA;
    sprite_attributes[sprite_number].x = info->x;
    sprite_attributes[sprite_number].y = set_y(info->y);

    info->x2 = info->x + 8;
    info->y2 = info->y + 8;

    //sprintf(temp, "S(%d,%d)", info->x2, info->y2);
    //vprint(temp, 19);

    return 1;
}

int helicopter(int sprite_number, SPRITE_STATE *info)
{
    int blade_right = info->flip;

    // top
    info->sprite_num = sprite_number;

    sprite_attributes[sprite_number].x = info->x;
    sprite_attributes[sprite_number].y = set_y(info->y);
    sprite_attributes[sprite_number].color_code = (blade_right) ? COLOR_WHITE : COLOR_TRANSPARENT;
    sprite_attributes[sprite_number].sprite_pattern = CHOPTER_BLADE_LEFT;

    sprite_attributes[sprite_number + 1].x = info->x + 8;
    sprite_attributes[sprite_number + 1].y = set_y(info->y);
    sprite_attributes[sprite_number + 1].color_code = COLOR_WHITE;
    sprite_attributes[sprite_number + 1].sprite_pattern = (blade_right) ? CHOPTER_BLADE_RIGHT_MID : CHOPTER_BLADE_LEFT_MID;

    sprite_attributes[sprite_number + 2].x = info->x + 16;
    sprite_attributes[sprite_number + 2].y = set_y(info->y);
    sprite_attributes[sprite_number + 2].color_code = (blade_right) ? COLOR_TRANSPARENT : COLOR_WHITE;
    sprite_attributes[sprite_number + 2].sprite_pattern = CHOPTER_BLADE_RIGHT;

    if (info->going_left)
    {
        // left front
        sprite_attributes[sprite_number + 3].x = info->x;
        sprite_attributes[sprite_number + 3].y = set_y(info->y + 8);
        sprite_attributes[sprite_number + 3].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 3].sprite_pattern = LCHOPTER_FRONT;

        // left mid
        sprite_attributes[sprite_number + 4].x = info->x + 8;
        sprite_attributes[sprite_number + 4].y = set_y(info->y + 8);
        sprite_attributes[sprite_number + 4].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 4].sprite_pattern = LCHOPTER_MID;

        // left tail
        sprite_attributes[sprite_number + 5].x = info->x + 16;
        sprite_attributes[sprite_number + 5].y = set_y(info->y + 8);
        sprite_attributes[sprite_number + 5].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 5].sprite_pattern = (info->flip) ? LCHOPTER_ROTOR_UP : LCHOPTER_ROTOR_SIDE;
    }
    else
    {
        // right tail
        sprite_attributes[sprite_number + 3].x = info->x;
        sprite_attributes[sprite_number + 3].y = set_y(info->y + 8);
        sprite_attributes[sprite_number + 3].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 3].sprite_pattern = (info->flip) ? CHOPTER_ROTOR_UP : CHOPTER_ROTOR_SIDE;

        // right mid
        sprite_attributes[sprite_number + 4].x = info->x + 8;
        sprite_attributes[sprite_number + 4].y = set_y(info->y + 8);
        sprite_attributes[sprite_number + 4].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 4].sprite_pattern = CHOPTER_MID;

        // right front
        sprite_attributes[sprite_number + 5].x = info->x + 16;
        sprite_attributes[sprite_number + 5].y = set_y(info->y + 8);
        sprite_attributes[sprite_number + 5].color_code = COLOR_WHITE;
        sprite_attributes[sprite_number + 5].sprite_pattern = CHOPTER_FRONT;
    }

    info->x2 = sprite_attributes[sprite_number + 5].x + 8;
    info->y2 = sprite_attributes[sprite_number + 5].y + 8;
    return 6;
}


int collision_detect(int sprite_num, SPRITE_STATE *all, int debug)
{
    int collision = -1;
    int a1, a2, b1, b2, in_x_bound, in_y_bound;
    SPRITE_STATE my_sprite;
    char temp[180];


    if (debug)
        for (int i=0; i<10; i++)
            vprint("               ", i);

    my_sprite = all[sprite_num];

    for(int i=0; i<32; i++)
    {

        if (sprite_num == i)
            continue;

        if (! all[i].enable)
        {
            continue;
        }
        

        a1 = my_sprite.x;
        a2 = my_sprite.x2;

        b1 = my_sprite.y;
        b2 = my_sprite.y2;

        if (debug)
        {
            sprintf(temp, "shot %02d\\[%d,%d][%d,%d]   ", all[i].sprite_num, b1, a2, b2);
            vprint(temp, 3);
            sprintf(temp, "type %02d/[%d,%d][%d,%d]   ",  all[i].sprite_type, all[i].x, all[i].y, all[i].x2, all[i].y2);
            vprint(temp, 5);
            // mygetchar();
        }
        in_x_bound = (all[i].x2 >= a1) && (all[i].x2 <= a2);
        in_y_bound = (all[i].y2 >= b1) && (all[i].y2 <= b2);

        if (in_x_bound && in_y_bound)
        {
            if (debug)
            {
                sprintf(temp, "*********[%d] type: %02d  ", i, all[i].sprite_type);
                vprint(temp, 6);
            }
            collision = all[i].sprite_num;
            break;
        }
    }

    if(debug)
        mygetchar();

    return collision;
}

void reverse_sprites(void)
{
    static SPRITE_ATTRIBUTE reversed[32];
    memcpy(reversed, sprite_attributes, 32*sizeof(SPRITE_ATTRIBUTE));

    int i=0;
    for (int s = 32-1; s>0; s--)
    {
        if (sprite_attributes[s].y == SPRITE_TERMINATOR)
            continue;

        reverse_attributes[i] = sprite_attributes[s];
        i++;
    }

}

void identify_sprite(int sprite_type, char *temp)
{
    switch (sprite_type)
    {
    case SPRITE_TYPE_BOMB:
        strcpy(temp, "Bomb        ");
        break;
    case SPRITE_TYPE_CHUTE:
        strcpy(temp, "Chute       ");
        break;
    case SPRITE_TYPE_EXPLODE:
        strcpy(temp, "Explode     ");
        break;
    case SPRITE_TYPE_HELICOPTER:
        strcpy(temp, "Helicopter  ");
        break;
    case SPRITE_TYPE_JET:
        strcpy(temp, "Jet         ");
        break;
    case SPRITE_TYPE_MAN:
        strcpy(temp, "man         ");
        break;
    case SPRITE_TYPE_SHOT:
        strcpy(temp, "Shot        ");
        break;
    case SPRITE_TYPE_NONE:
    default:
        sprintf(temp, "Unknown %d  ", sprite_type);
        break;
    }

}
