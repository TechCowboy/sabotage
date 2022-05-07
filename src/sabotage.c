#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msx.h>
#include <msx/gfx.h>
#include <arch/z80.h>
#include <eos.h>
#include "charset.h"
#include "colorset.h"
#include "spriteset.h"
#include "vdp_registers.h"
#include "colorset.h"
#include "graphics.h"
#include "sound.h"
#include "charctrl.h"
#include "spritectrl.h"
#include "misc.h"
#include "readkeyboard.h"
#include "readjoystick.h"

unsigned char title[940];

/*
int collision_detect(int sprite_number)
{
    int min_x=257, 
        max_x=-1, 
        min_y=193, 
        max_y=-1;
    int sprite_collision = -1;

    if (sprite_number >= 0)
    {
        for(int s=0; s<32; s++)
        {
            if (sprite_list[s] == -1)
                break;

            if (sprite_list[s] == sprite_number)
            {
                min_x = MIN(min_x, (int) sprite_attributes[s].x);
                max_x = MAX(max_x, (int) sprite_attributes[s].x+8);
                min_y = MIN(min_y, (int) sprite_attributes[s].y);
                max_y = MAX(max_y, (int) sprite_attributes[s].y+8);
            }
        }

        for(int s=0; s<32; s++)
        {
            if (sprite_list[s] == -1)
                break;

            if (sprite_attributes[s].y == SPRITE_TERMINATOR)
                break;
            
            if (sprite_list[s] == sprite_number)
                continue;

            if (sprite_attributes[s].color_code == COLOR_TRANSPARENT)
                continue;
            
            if (((int) sprite_attributes[s].x >= min_x) && ((int) sprite_attributes[s].x <= max_x) &&
                ((int) sprite_attributes[s].y >= min_y) && ((int) sprite_attributes[s].y <= max_y))
            {
                sprite_collision = sprite_list[s];
                break;

            }

        }
    }

    return sprite_collision;
}
*/


/******************************************/
/******************************************/
/***************  MAIN  *******************/
/******************************************/
/******************************************/

typedef struct _SHOT_ANGLE
{
    int x;
    int y;
    int inc_x;
    int inc_y;
} SHOT_ANGLE;

void main()
{
    char temp[80];
    int  answer;
    bool steerable = false;
    int right_start_x = 0;
    int left_start_x = 256 - 8 * 3;
    int direction = 1;
    int n;
    int start, stop, inc, sprite, wave_done, sprite_collision;
    bool left = false;
    int fire;
    char c;
    SPRITE_STATE all_sprites[32];
    int shot_delay = 15;
    WAVE waves[] = 
    {
        {2, 24, CHOPTER_SLOW, 1, 0, 0},
        {4, 24, CHOPTER_SLOW, 1, 0, 0},
        {4, 36, CHOPTER_MED,  2, 0, 0},
        {4, 36, CHOPTER_MED,  2, 1, 1},
        {4, 50, CHOPTER_FAST, 4, 4, 4}
    };
    int rotation = 3;
    SHOT_ANGLE shot_angle[] =
        {
            {128, 178, -16,  0},  // 0
            {125, 179, -8,  -8},  // 1
            {126, 178, -4,  -4},  // 2
            {127, 175,  0, -16},  // 3  UP
            {128, 178,  4,  -4},  // 4
            {129, 179,  8,  -8},  // 5
            {128, 178,  16,  0}   // 6
        };

    srand(1);

    mode_text();
    clr(' ');
    vprint("Please Wait...", 0);


    /******** INIT ******************/

    sound_init();
    init_keyboard();
    
    for(int i=0; i<32; i++)
    {
        all_sprites[i].flip = rand() < 16384 ? true : false;
        all_sprites[i].enable = false;
        all_sprites[i].state = OFF_SCREEN;
        all_sprites[i].sprite = NO_SPRITE;
        all_sprites[i].jumpers = 1;
        all_sprites[i].going_left = left;
        all_sprites[i].appearance_wait = my_rand(8, 256 - 8);
        all_sprites[i].y = SPRITE_TERMINATOR;
        all_sprites[i].inc_x = CHOPTER_SLOW;
        all_sprites[i].inc_y = 0;

        left = ! left;
    }


    answer = introduction(0);
    steerable = ((answer == 'Y') || (answer == 'y'));

    mode_graphics_ii();
    clr(' ');

    create_text_ground();
    create_text_turret(rotation);

    int flyers = 0;
    int add_sprite;
    int emergency_stop = 0;

    // *************PREPARE THIS WAVE

    int hel_pos = 8*3;

    for (int wave=0; wave<4; wave++)
    {

        sprintf(title, "WAVE %d", wave+1);
        vprint(title,0);

        left = my_rand(0,1);
        hel_pos = waves[wave].heli_height;

        for(int i=0; i < 32; i++)
        {
            all_sprites[i].enable = false;
            all_sprites[i].state = OFF_SCREEN;
        }

        flyers = 0;
        for (int i = 0; i < waves[wave].helicopters; i++)
        {
            all_sprites[flyers].sprite_type = SPRITE_TYPE_HELICOPTER;
            all_sprites[flyers].enable = true;
            all_sprites[flyers].state = OFF_SCREEN;
            all_sprites[flyers].jumpers = waves[wave].jumpers;
            all_sprites[flyers].inc_x = waves[wave].heli_speed;
            all_sprites[flyers].inc_y = 0;

            all_sprites[flyers].going_left = left;

            if (all_sprites[flyers].going_left)
                all_sprites[flyers].x = left_start_x;
            else
                all_sprites[flyers].x = right_start_x;

            all_sprites[flyers].y = set_y(hel_pos);
            all_sprites[flyers].appearance_wait = my_rand(8, 80);
            all_sprites[flyers].jump_wait = my_rand(32, 256 - 32);
            hel_pos += 16;

            flyers++;
            left = !left;
        }

        int jet_pos = 0;

        left = my_rand(0,1);
        for (int i = 0; i < waves[wave].jets; i++)
        {
            all_sprites[flyers].sprite_type = SPRITE_TYPE_JET;
            all_sprites[flyers].enable = true;
            all_sprites[flyers].state = OFF_SCREEN;
            all_sprites[flyers].bombs = waves[wave].bombs_per_jet;
            all_sprites[flyers].inc_x = JET_SPEED;
            all_sprites[flyers].inc_y = 0;

            all_sprites[flyers].going_left = left;

            if (all_sprites[flyers].going_left)
                all_sprites[flyers].x = left_start_x;
            else
                all_sprites[flyers].x = right_start_x;

            all_sprites[flyers].y = set_y(jet_pos);
            all_sprites[flyers].appearance_wait = my_rand(8, 80);
            all_sprites[flyers].bomb_wait = my_rand(32, 64) / JET_SPEED;
            jet_pos += 16;
            flyers++;

            left = !left;
        }

        
        while(true)
        {
            rotation = read_keyboard(rotation);
            fire = (rotation & 16384);

            rotation = read_joystick(rotation);
            fire = (rotation & 16384) | fire; // joystick or keyboard

            rotation = rotation & ~16384;
            create_text_turret(rotation);

            if (--shot_delay > 0)
                fire = 0;
            else   
            {
                fire = 1;
                shot_delay = 15;
            }

            sprite = 0;
            for (int i = 0; i < 32; i++)
                sprite_attributes[i].y = SPRITE_TERMINATOR;

            wave_done = true;
            for(int i=0; i <32; i++)
            {
                // go through the sprites
                if (all_sprites[i].sprite_type == SPRITE_TYPE_HELICOPTER)
                {
                    if (all_sprites[i].enable)
                    {
                        wave_done = false;
                        /* ADD HELICOPTER SPRITES */
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            n = helicopter(sprite, &all_sprites[i]);
                            all_sprites[i].sprite = sprite;

                            sprite += n;
                            if (sprite >= 31)
                            {
                                all_sprites[i].sprite = -1;
                                all_sprites[i].state = OFF_SCREEN;
                                all_sprites[i].enable = false;
                                sprite -= n;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }
                        }
                        else
                        {
                            if (all_sprites[i].appearance_wait > 0)
                            {
                                all_sprites[i].appearance_wait--;
                            }
                            else
                            {
                                all_sprites[i].state = ON_SCREEN;
                                all_sprites[i].appearance_wait = 0;
                            }
                        }
                    } // if hel enable
                } // if hel

                /* ADD JET SPRITES */
                if (all_sprites[i].sprite_type == SPRITE_TYPE_JET)
                {
                    if (all_sprites[i].enable)
                    {
                        wave_done = false;
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            n = jet(sprite, &all_sprites[i]);
                            all_sprites[i].sprite = sprite;

                            sprite += n;
                            if (sprite >= 31)
                            {
                                all_sprites[i].sprite = -1;
                                all_sprites[i].state = OFF_SCREEN;
                                all_sprites[i].enable = false;
                                sprite -= n;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }
                        }
                        else
                        {
                            if (all_sprites[i].appearance_wait > 0)
                            {
                                all_sprites[i].appearance_wait--;
                            }
                            else
                            {
                                all_sprites[i].state = ON_SCREEN;
                                all_sprites[i].appearance_wait = 0;
                            }
                        }
                    } // if enable
                } // if jet

                if (all_sprites[i].sprite_type == SPRITE_TYPE_MAN)
                {
                    /* ADD MAN SPRITES */
                    if (all_sprites[i].enable)
                    {
                        wave_done = false;
                        n = parachuter(sprite, &all_sprites[i]);
                        all_sprites[i].sprite = sprite;
                        sprite += n;
                        if (sprite >= 31)
                        {
                            all_sprites[i].sprite = -1;
                            all_sprites[i].state = OFF_SCREEN;
                            all_sprites[i].enable = false;
                            sprite -= n;
                            sprintf(title, "TOO MANY SPRITES!");
                            vprint(title, 13);
                            break;
                        }

                    } // if enable
                } // if man

                if (all_sprites[i].sprite_type == SPRITE_TYPE_SHOT)
                {
                    if (all_sprites[i].enable)
                    {
                        /* ADD SHOT SPRITES */
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            wave_done = false;
                            // sprintf(title, "SHOT! (%d,%d) sprite %d", shot_sprites[i].x, shot_sprites[i].y, sprite);
                            // vprint(title, 4);
                            n = shot(sprite, &all_sprites[i]);
                            all_sprites[i].sprite = sprite;

                            // emergency_stop = 1;
                            sprite += n;
                            if (sprite >= 31)
                            {
                                sprite -= n;
                                all_sprites[i].sprite = -1;
                                all_sprites[i].enable = false;
                                all_sprites[i].state = OFF_SCREEN;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }
                        } // if on screen

                    } // if enable
                } // if shot

                if (all_sprites[i].sprite_type == SPRITE_TYPE_BOMB)
                {
                    /* ADD BOMB SPRITES */
                    if (all_sprites[i].enable)
                    {
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            wave_done = false;
                            n = bomb(sprite, &all_sprites[i]);
                            all_sprites[i].sprite = sprite;

                            sprite += n;
                            if (sprite >= 31)
                            {
                                sprite -= n;
                                all_sprites[i].sprite = -1;
                                all_sprites[i].enable = false;
                                all_sprites[i].state = OFF_SCREEN;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }

                        } // if on screen
                    } // if enable
                } // if bomb

                all_sprites[i].flip = !all_sprites[i].flip;
            } // for 

            /* DISPLAY OUR SPRITES */

            vwrite(sprite_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));
            //reverse_sprites();
            //vwrite(reverse_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));


            // *****************************************************
            // wave is done if there are no active sprites on screen
            // *****************************************************
            if (wave_done)
                break;

    
            for (int i=0; i<32; i++)
            {
                if (all_sprites[i].state == OFF_SCREEN)
                    continue;

                if (! all_sprites[i].enable)
                    continue;


                // *******************************
                // HELICOPTER
                // *******************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_HELICOPTER)
                {
                    add_sprite = 0;
                    // **********************************
                    // SEE IF IT'S TIME TO DROP A MAN
                    // **********************************
                    if (   (all_sprites[i].state == ON_SCREEN) 
                        && (all_sprites[i].jumpers > 0))
                    {
                        all_sprites[i].jump_wait--;
                        if (all_sprites[i].jump_wait <= 0)
                        {
                            add_sprite = 1;
                            all_sprites[i].jumpers--;

                            // if there are more jumpers, set up a random jump time
                            if (all_sprites[i].jumpers > 0)
                            {
                                all_sprites[i].jump_wait = my_rand(10, 50);
                            }
                            else
                            {
                                // no more jumpers
                                all_sprites[i].jumpers = 0;
                                all_sprites[i].jump_wait = 0;
                            }
                        }

                        if (add_sprite)
                        {
                            int man = 0;
                            for(man=0; man<32; man++)
                            {
                                if (! all_sprites[man].enable)
                                    break;
                            }

                            if (man < 32)
                            {
                                all_sprites[man].sprite_type = SPRITE_TYPE_MAN;
                                all_sprites[man].enable = true;
                                all_sprites[man].x = all_sprites[i].x + 8;
                                all_sprites[man].y = set_y(all_sprites[i].y + 16);
                                all_sprites[man].state = JUMPED;
                            }
                        } // if add_man
                    } // if hel on_screen
                } // if helicopter

                // ****************************************
                // JET
                // ****************************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_JET)
                {
                    add_sprite = 0;

                    // ********************************
                    // SEE IF IT'S TIME TO DROP A BOMB
                    // ********************************
                    if ((all_sprites[i].state == ON_SCREEN) && (all_sprites[i].bombs > 0))
                    {
                        all_sprites[i].bomb_wait--;
                        if (all_sprites[i].bomb_wait <= 0)
                        {
                            add_sprite = 1;
                            all_sprites[i].bombs--;

                            // if there are more jumpers, set up a random jump time
                            if (all_sprites[i].bombs > 0)
                            {
                                all_sprites[i].bomb_wait = my_rand(10, 50);
                            }
                            else
                            {
                                // no more jumpers
                                all_sprites[i].bombs = 0;
                                all_sprites[i].bomb_wait = 0;
                            }
                        }

                        if (add_sprite)
                        {
                            int bomb = 0;
                            for (bomb = 0; bomb < 32; bomb++)
                            {
                                if (! all_sprites[bomb].enable)
                                    break;
                            }

                            if (bomb < 32)
                            {
                                all_sprites[bomb].sprite_type = SPRITE_TYPE_BOMB;
                                all_sprites[bomb].enable = true;
                                all_sprites[bomb].x = all_sprites[i].x + 8;
                                all_sprites[bomb].y = set_y(all_sprites[i].y + 16);
                                all_sprites[bomb].state = DROPPED;
                            }
                        } // if add_bomb
                    } // if on screen and bomb
                } // if jet
            } // for

            add_sprite = 0;


            // ******************************************************
            // *** COLLISION DETECTION
            // ******************************************************
            for (int i=0; i<32; i++)
            {
                if (all_sprites[i].sprite_type != SPRITE_TYPE_SHOT)
                    continue;

                if (all_sprites[i].state == ON_SCREEN)
                {
                    sprite_collision = collision_detect(all_sprites[i].sprite, all_sprites);
                    if (sprite_collision != OFF_SCREEN)
                    {

                    }
                }
            
            }
            
            
            for(int i=0; i<32; i++)
            {
                // *****************************************
                // MOVE THE MEN
                // *****************************************

                if (all_sprites[i].sprite_type == SPRITE_TYPE_MAN)
                {
                    if (all_sprites[i].state == OFF_SCREEN)
                        continue;

                    if (all_sprites[i].state == FLOATING)
                        all_sprites[i].y = set_y(all_sprites[i].y + FLOATING_SPEED);
                    else
                        all_sprites[i].y = set_y(all_sprites[i].y + FALLING_SPEED);

                    if (all_sprites[i].state <= JUMPED3)
                        all_sprites[i].state++;

                    if (all_sprites[i].y > BOTTOM_SCREEN_Y - 16)
                    {
                        all_sprites[i].enable = false;
                        all_sprites[i].state = OFF_SCREEN;
                        int char_x;
                        int char_y;
                        all_sprites[i].y = set_y(all_sprites[i].y + 8);

                        // replace the man sprite with a man character
                        char_x = all_sprites[i].x / 8;
                        char_y = all_sprites[i].y / 8;
                        vpoke(VRAM_NAME_TABLE + char_y * total_columns + char_x + 1, CHAR_MAN);
                    }
                } // if man


                // **************************************************
                // MOVE THE HELICOPTERS
                // **************************************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_HELICOPTER)
                {
                    if (all_sprites[i].enable)
                    {
                        // **********************************************
                        // hide sprites going off screen
                        // **********************************************
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            if (all_sprites[i].going_left)
                            {
                                if (all_sprites[i].x <= 0)
                                {
                                    all_sprites[i].state == OFF_SCREEN;
                                    all_sprites[i].enable = false;
                                    all_sprites[i].jumpers = 0;
                                }
                            }
                            else
                            {
                                if (all_sprites[i].x >= 256 - 24)
                                {
                                    all_sprites[i].state == OFF_SCREEN;
                                    all_sprites[i].enable = false;
                                    all_sprites[i].jumpers = 0;
                                }
                            }
                        }
                    }
                } // if helicopter 

                if (all_sprites[i].sprite_type == SPRITE_TYPE_JET)
                {
                    if (all_sprites[i].enable)
                    {
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            if (all_sprites[i].going_left)
                            {
                                if (all_sprites[i].x <= 0)
                                {
                                    all_sprites[i].state == OFF_SCREEN;
                                    all_sprites[i].enable = false;
                                    all_sprites[i].bombs = 0;
                                }
                            }
                            else
                            {
                                if (all_sprites[i].x >= 256 - 24)
                                {
                                    all_sprites[i].state == OFF_SCREEN;
                                    all_sprites[i].enable = false;
                                    all_sprites[i].bombs = 0;
                                }
                            }
                        }
                    }
                } // if jet

                if (all_sprites[i].sprite_type == SPRITE_TYPE_BOMB)
                {
                    if (   (all_sprites[i].x <= 0) 
                        || (all_sprites[i].x >= 256 - 8) 
                        || (all_sprites[i].y >= 192 - 8))
                    {
                        all_sprites[i].state = OFF_SCREEN;
                        all_sprites[i].enable = false;
                    }
                } // if bomb

                if (   (all_sprites[i].sprite_type == SPRITE_TYPE_HELICOPTER)
                    || (all_sprites[i].sprite_type == SPRITE_TYPE_JET))
                {
                    if (all_sprites[i].enable)
                    {
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            if (all_sprites[i].going_left)
                                all_sprites[i].x += -all_sprites[i].inc_x;
                            else
                                all_sprites[i].x += all_sprites[i].inc_x;
                        }
                    }
                } // if helicopter or jet

                if (all_sprites[i].sprite_type == SPRITE_TYPE_BOMB)
                {
                    if (all_sprites[i].enable)
                    {
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            if (all_sprites[i].going_left)
                                all_sprites[i].x += -all_sprites[i].inc_x;
                            else
                                all_sprites[i].x += all_sprites[i].inc_x;
                            all_sprites[i].y += all_sprites[i].inc_y;
                        }
                    }
                } // if bomb

                if (all_sprites[i].sprite_type == SPRITE_TYPE_SHOT)
                {
                    if (all_sprites[i].enable)
                    {
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            if (all_sprites[i].going_left)
                                all_sprites[i].x += -all_sprites[i].inc_x;
                            else
                                all_sprites[i].x += all_sprites[i].inc_x;

                            all_sprites[i].y += all_sprites[i].inc_y;

                            if ((all_sprites[i].x < 0) ||
                                (all_sprites[i].x > 256 - 8) ||
                                (all_sprites[i].y < 0))
                            {
                                all_sprites[i].enable = false;
                                all_sprites[i].state = OFF_SCREEN;
                            }
                        } // if on screen..
                    }
                } // if shot

                // ********************************
                // SEE IF A SHOT HAS BEEN FIRED
                // ********************************
                if (fire)
                {
                    int shot = 0;
                    for (shot = 0; shot < 32; shot++)
                    {
                        if (! all_sprites[shot].enable)
                            break;
                    }

                    if (shot < 32)
                    {
                        all_sprites[shot].enable = true;
                        all_sprites[shot].x = shot_angle[rotation].x;
                        all_sprites[shot].y = shot_angle[rotation].y;
                        all_sprites[shot].inc_x = shot_angle[rotation].inc_x;
                        all_sprites[shot].inc_y = shot_angle[rotation].inc_y;
                        all_sprites[shot].state = ON_SCREEN;
                        all_sprites[shot].sprite = -1;
                    }
                    fire = 0;
                } // if fire
            } // for

        } // while true   
    } // for wave
    end_game();
    
}
