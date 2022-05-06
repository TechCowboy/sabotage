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

void main()
{
    char temp[80];
    int  answer;
    bool steerable = false;
    int right_start_x = 0;
    int left_start_x = 256 - 8 * 3;
    int rotation = 0;
    bool left = false;
    int fire;
    char c;
    SPRITE_STATE hel_sprites[32];
    SPRITE_STATE  jet_sprites[32];
    SPRITE_STATE  man_sprites[32];
    SPRITE_STATE  shot_sprites[32];
    SPRITE_STATE  bomb_sprites[32];
    WAVE waves[] = 
    {
        {2, 24, CHOPTER_SLOW, 1, 0, 0},
        {4, 24, CHOPTER_SLOW, 1, 0, 0},
        {4, 36, CHOPTER_MED,  2, 0, 0},
        {4, 36, CHOPTER_MED,  2, 1, 1},
        {4, 50, CHOPTER_FAST, 4, 4, 4}
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
        hel_sprites[i].flip = rand() < 16384 ? true : false;
        hel_sprites[i].enable = false;
        hel_sprites[i].state = OFF_SCREEN;
        hel_sprites[i].sprite = NO_SPRITE;
        hel_sprites[i].jumpers = 1;
        hel_sprites[i].going_left = left;
        hel_sprites[i].appearance_wait = my_rand(8, 256 - 8);
        hel_sprites[i].y = SPRITE_TERMINATOR;
        hel_sprites[i].inc_x = CHOPTER_SLOW;
        hel_sprites[i].inc_y = 0;

        
        jet_sprites[i].enable = false;
        jet_sprites[i].state = OFF_SCREEN;
        jet_sprites[i].sprite = NO_SPRITE;
        jet_sprites[i].bombs = 1;
        jet_sprites[i].going_left = left;
        jet_sprites[i].appearance_wait = my_rand(8, 256 - 8);
        jet_sprites[i].y = SPRITE_TERMINATOR;
        jet_sprites[i].inc_x = JET_SPEED;
        jet_sprites[i].inc_y = 0;

        man_sprites[i].flip = rand() < 16384 ? true : false;
        man_sprites[i].enable = false;
        man_sprites[i].state = OFF_SCREEN;
        man_sprites[i].sprite = NO_SPRITE;
        man_sprites[i].y = SPRITE_TERMINATOR;
        hel_sprites[i].inc_x = 0; // unused
        hel_sprites[i].inc_y = 0;

        shot_sprites[i].enable = false;
        shot_sprites[i].state = OFF_SCREEN;
        shot_sprites[i].sprite = NO_SPRITE;
        shot_sprites[i].y = SPRITE_TERMINATOR;
        shot_sprites[i].inc_x = SHOT_X_SPEED;
        shot_sprites[i].inc_y = SHOT_Y_SPEED;

        bomb_sprites[i].enable = false;
        bomb_sprites[i].state = OFF_SCREEN;
        bomb_sprites[i].sprite = NO_SPRITE;
        bomb_sprites[i].y = SPRITE_TERMINATOR;
        bomb_sprites[i].inc_x = BOMB_X_SPEED;
        bomb_sprites[i].inc_y = BOMB_Y_SPEED;

        left = ! left;
    }


    answer = introduction(0);
    steerable = ((answer == 'Y') || (answer == 'y'));

    mode_graphics_ii();
    clr(' ');

    create_text_ground();
    rotation=0;
    create_text_turret(rotation);

    //test_char_color();
    //getchar();

/*
    for(;;)
    {
        rotation = read_joystick(rotation);
        fire = (rotation & 16384) > 0;
        rotation = rotation & (~16384);
        create_text_turret(rotation);
        sprintf(title, "rotation: %d  fire: %d", rotation, fire);
        vprint(title, 1);
    }
*/

    int helicopters = 2;
    int jets        = 2;
    int shots       = 4;

    int emergency_stop = 0;

    // *************PREPARE THIS WAVE

    int hel_pos = 8*3;



    int direction = 1;
    int n;
    int start, stop, inc, sprite, wave_done;
    for (int wave=0; wave<4; wave++)
    {
        sprintf(title, "WAVE %d", wave+1);
        vprint(title,0);

        left = my_rand(0,1);
        hel_pos = waves[wave].heli_height;

        for (int i = 0; i < waves[wave].helicopters; i++)
        {
            hel_sprites[i].enable = true;
            hel_sprites[i].state = OFF_SCREEN;
            hel_sprites[i].jumpers = waves[wave].jumpers;
            hel_sprites[i].inc_x = waves[wave].heli_speed;
            hel_sprites[i].inc_y = 0;

            hel_sprites[i].going_left = left;

            if (hel_sprites[i].going_left)
                hel_sprites[i].x = left_start_x;
            else
                hel_sprites[i].x = right_start_x;

            hel_sprites[i].y = set_y(hel_pos);
            hel_sprites[i].appearance_wait = my_rand(8, 80);
            hel_sprites[i].jump_wait = my_rand(32, 256 - 32);
            hel_pos += 16;

            left = ! left;
        }

        int jet_pos = 0;

        left = my_rand(0,1);
        for (int i = 0; i < waves[wave].jets; i++)
        {
            jet_sprites[i].enable = true;
            jet_sprites[i].state = OFF_SCREEN;
            jet_sprites[i].bombs = waves[wave].bombs_per_jet;
            jet_sprites[i].inc_x = JET_SPEED;
            jet_sprites[i].inc_y = 0;

            jet_sprites[i].going_left = left;

            if (jet_sprites[i].going_left)
                jet_sprites[i].x = left_start_x;
            else
                jet_sprites[i].x = right_start_x;

            jet_sprites[i].y = set_y(jet_pos);
            jet_sprites[i].appearance_wait = my_rand(8, 80);
            jet_sprites[i].bomb_wait = my_rand(32, 64) / JET_SPEED;
            jet_pos += 16;
            left = ! left;
        }

        
        while(true)
        {
            rotation = read_keyboard(rotation);
            fire = (rotation & 16384);
            rotation=rotation & ~16384;

            rotation = read_joystick(rotation);
            fire = (rotation & 16384);
            rotation = rotation & ~16384;

            create_text_turret(rotation);

            // ********************************************************
            // do this twice so that we have up to 8 sprites on screen
            // ********************************************************
            for (int repeat=0; repeat<2; repeat++)
            {
                sprite = 0;
                for (int i = 0; i < 32; i++)
                    sprite_attributes[i].y = SPRITE_TERMINATOR;

                direction = -direction;
                if (direction == 1)
                {
                    start = 0;
                    stop= 32;
                    inc = 1;
                } else
                {
                    start = 31;
                    stop = -1;
                    inc = -1;
                }

                wave_done = true;
                for(int i=start; i != stop; i += inc)
                {
                    // go through the sprites
                    if (hel_sprites[i].enable)
                    {
                        wave_done = false;
                        /* ADD HELICOPTER SPRITES */
                        if (hel_sprites[i].state == ON_SCREEN)
                        {
                            n = helicopter(sprite, hel_sprites[i]);
                            
                            sprite += n;
                            if (sprite >= 31)
                            {
                                sprite -= n;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }
                            hel_sprites[i].sprite = sprite;
                        } else
                        {
                            if (hel_sprites[i].appearance_wait > 0)
                            {
                                hel_sprites[i].appearance_wait--;
                            } else
                            {
                                hel_sprites[i].state = ON_SCREEN;
                                hel_sprites[i].appearance_wait = 0;
                            }

                        }
                    }

                    /* ADD JET SPRITES */
                    if (jet_sprites[i].enable)
                    {
                        wave_done = false;
                        if (jet_sprites[i].state == ON_SCREEN)
                        {
                            n = jet(sprite, jet_sprites[i]);

                            sprite += n;
                            if (sprite >= 31)
                            {
                                sprite -=n;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }
                            jet_sprites[i].sprite = sprite;
                        }
                        else
                        {
                            if (jet_sprites[i].appearance_wait > 0)
                            {
                                jet_sprites[i].appearance_wait--;
                            }
                            else
                            {
                                jet_sprites[i].state = ON_SCREEN;
                                jet_sprites[i].appearance_wait = 0;
                            }
                        }
                    }

                    /* ADD MAN SPRITES */
                    if (man_sprites[i].enable)
                    {
                        wave_done = false;
                        n = parachuter(sprite, man_sprites[i]);

                        sprite += n;
                        if (sprite >= 31)
                        {
                            sprite -= n;
                            sprintf(title, "TOO MANY SPRITES!");
                            vprint(title, 13);
                            break;
                        }
                        man_sprites[i].sprite = sprite;
                    }

                    /* ADD SHOT SPRITES */
                    if (shot_sprites[i].state == ON_SCREEN)
                    {
                        wave_done = false;
                        sprintf(title, "SHOT! (%d,%d) sprite %d", shot_sprites[i].x, shot_sprites[i].y, sprite);
                        vprint(title, 4);
                        n = shot(sprite, shot_sprites[i]);

                        sprite += n;
                        if (sprite >= 31)
                        {
                            sprite -= n;
                            sprintf(title, "TOO MANY SPRITES!");
                            vprint(title, 13);
                            break;
                        }
                        shot_sprites[i].sprite = sprite;
                    }

                    /* ADD BOMB SPRITES */
                    if (bomb_sprites[i].state == ON_SCREEN)
                    {
                        wave_done = false;
                        n = bomb(sprite, bomb_sprites[i]);

                        sprite += n;
                        if (sprite >= 31)
                        {
                            sprite -= n;
                            sprintf(title, "TOO MANY SPRITES!");
                            vprint(title, 13);
                            break;
                        }
                        bomb_sprites[i].sprite = sprite;
                    }

                } // for start

                /* DISPLAY OUR SPRITES */
                vwrite(sprite_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));

                if (emergency_stop)
                {
                    sprintf(title, "Total sprites: %d", sprite);
                    vprint(title, 12);
                    strcpy(title, ".");
                    vprint(title, 13);
                    for(int w =0; w<sprite+2; w++)
                    {
                        sprintf(temp, "%d(%d,%d) ", w, (int)sprite_attributes[w].x, (int)sprite_attributes[w].x);
                        strcat(title, temp);
                        vprint(title,13);
                    }
                    vprint(title, 13);
                    getchar();
                }
            } // for repeat

            // *****************************************************
            // wave is done if there are no active sprites on screen
            // *****************************************************
            if (wave_done)
                break;

            // *****************************************************
            // flip is used for failing parachutes and 
            // helicopter rotors. 
            // *****************************************************
            for(int i=0; i<32; i++)
            {
                hel_sprites[i].flip = ! hel_sprites[i].flip;
                jet_sprites[i].flip = ! jet_sprites[i].flip;
                man_sprites[i].flip = ! man_sprites[i].flip;
            }

        /*
            // CHECK HELICOPTER FOR COLLISONS
            int sprite_collision;
            for (int i=0; i<32; i++)
            {
                if (hel_sprites[i].sprite == OFF_SCREEN)
                    break;
                sprite_collision = collision_detect(hel_sprites[i].sprite);
                if (sprite_collision != OFF_SCREEN)
                {
                    // dead
                }
            }

            // CHECK FOR JET COLLISIONS
            for (int i = 0; i < 32; i++)
            {
                if (jet_sprites[i].sprite == OFF_SCREEN)
                    break;

                sprite_collision = collision_detect(jet_sprites[i].sprite);
                if (sprite_collision != OFF_SCREEN)
                {
                    // dead
                }
            }
    */


            int add_sprite;
    
            for (int i=0; i<32; i++)
            {
                if (hel_sprites[i].state == OFF_SCREEN)
                    continue;

                add_sprite = 0;
                // **********************************
                // SEE IF IT'S TIME TO DROP A MAN
                // **********************************
                if ((hel_sprites[i].state == ON_SCREEN) && (hel_sprites[i].jumpers > 0))
                {
                    hel_sprites[i].jump_wait--;
                    if (hel_sprites[i].jump_wait <= 0)
                    {
                        add_sprite = 1;
                        hel_sprites[i].jumpers--;

                        // if there are more jumpers, set up a random jump time
                        if (hel_sprites[i].jumpers > 0)
                        {
                            hel_sprites[i].jump_wait = my_rand(10, 50);
                        }
                        else
                        {
                            // no more jumpers
                            hel_sprites[i].jumpers = 0;
                            hel_sprites[i].jump_wait = 0;
                        }
                    }

                    if (add_sprite)
                    {
                        int man = 0;
                        for(man=0; man<32; man++)
                        {
                            if (man_sprites[man].state == OFF_SCREEN)
                                break;
                        }

                        if (man < 32)
                        {
                            man_sprites[man].enable = true;
                            man_sprites[man].x = hel_sprites[i].x+8;
                            man_sprites[man].y = set_y(hel_sprites[i].y+16);
                            man_sprites[man].state = JUMPED;
                        }
                    } // if add_man
                } // if hel on_screen
            }

            for(int i=0; i<32; i++)
            {
                if (jet_sprites[i].state == OFF_SCREEN)
                    continue;

                add_sprite = 0;

                // ********************************
                // SEE IF IT'S TIME TO DROP A BOMB
                // ********************************
                if ((jet_sprites[i].state == ON_SCREEN) && (jet_sprites[i].bombs > 0))
                {
                    jet_sprites[i].bomb_wait--;
                    if (jet_sprites[i].bomb_wait <= 0)
                    {
                        add_sprite = 1;
                        jet_sprites[i].bombs--;

                        // if there are more jumpers, set up a random jump time
                        if (jet_sprites[i].bombs > 0)
                        {
                            bomb_sprites[i].bomb_wait = my_rand(10, 50);
                        }
                        else
                        {
                            // no more jumpers
                            jet_sprites[i].bombs = 0;
                            jet_sprites[i].bomb_wait = 0;
                        }
                    }

                    if (add_sprite)
                    {
                        int bomb = 0;
                        for (bomb = 0; bomb < 32; bomb++)
                        {
                            if (bomb_sprites[bomb].state == OFF_SCREEN)
                                break;
                        }

                        if (bomb < 32)
                        {
                            bomb_sprites[bomb].enable = true;
                            bomb_sprites[bomb].x = jet_sprites[i].x+8;
                            bomb_sprites[bomb].y = set_y(jet_sprites[i].y + 16);
                            bomb_sprites[bomb].state = DROPPED;
                        }
                    } // if add_bomb
                } // if jet on_screen
            }

    /*

            for (int i=0; i<32; i++)
            {
                if (man_sprites[i].state != OFF_SCREEN)
                {
                    sprite_collision = collision_detect(man_sprites[i].state);
                    if (sprite_collision != OFF_SCREEN)
                    {
                        man_sprites[i].state = FALLING;

                        for (int h=0; h<32; h++)
                        {
                            if (hel_sprites[h].sprite == sprite_collision)
                            {
                                hel_sprites[h].sprite = OFF_SCREEN;
                                if (hel_sprites[h].going_left)
                                    hel_sprites[h].x = left_start_x;
                                else
                                    hel_sprites[h].x = right_start_x;
                            }

                            if (jet_sprites[h].sprite == sprite_collision)
                            {
                                jet_sprites[h].sprite = OFF_SCREEN;
                                if (jet_sprites[h].going_left)
                                    jet_sprites[h].x = left_start_x;
                                else
                                    jet_sprites[h].x = right_start_x;
                            }
                        }
                        release_sprites(sprite_collision);
                    }

                }
                
            }
            */

            // *****************************************
            // MOVE THE MEN
            // *****************************************
            for(int i=0; i<32; i++)
            {
                if (man_sprites[i].state == OFF_SCREEN)
                    continue;

                if (man_sprites[i].state == FLOATING)
                    man_sprites[i].y = set_y(man_sprites[i].y + FLOATING_SPEED);
                else
                    man_sprites[i].y = set_y(man_sprites[i].y + FALLING_SPEED);

                if (man_sprites[i].state <= JUMPED3)
                    man_sprites[i].state++;

                if (man_sprites[i].y > BOTTOM_SCREEN_Y - 16)
                {
                    man_sprites[i].enable = false;
                    man_sprites[i].state = OFF_SCREEN;
                    int char_x;
                    int char_y;
                    man_sprites[i].y = set_y(man_sprites[i].y + 8);
                    
                    // replace the man sprite with a man character
                    char_x = man_sprites[i].x / 8;
                    char_y = man_sprites[i].y / 8;
                    //vpoke(VRAM_NAME_TABLE + char_y * total_columns + char_x + 1, CHAR_MAN);
                }
            }


            // **************************************************
            // MOVE THE HELICOPTERS, JETS, BOMBS and SHOTS
            // **************************************************
            for (int i = 0; i < 32; i++)
            {
                // **********************************************
                // hide sprites going off screen
                // **********************************************
                if (hel_sprites[i].state == ON_SCREEN)
                {
                    if (hel_sprites[i].going_left)
                    {
                        if (hel_sprites[i].x <= 0)
                        {
                            hel_sprites[i].state == OFF_SCREEN;
                            hel_sprites[i].enable = false;
                            hel_sprites[i].jumpers = 0;
                        }
                    }
                    else
                    {
                        if (hel_sprites[i].x >= 256 - 24)
                        {
                            hel_sprites[i].state == OFF_SCREEN;
                            hel_sprites[i].enable = false;
                            hel_sprites[i].jumpers = 0;
                        }
                    }
                }

                if (jet_sprites[i].state == ON_SCREEN)
                {
                    if (jet_sprites[i].going_left)
                    {
                        if (jet_sprites[i].x <= 0)
                        {
                            jet_sprites[i].state == OFF_SCREEN;
                            jet_sprites[i].enable = false;
                            jet_sprites[i].bombs = 0;
                        }
                    }
                    else
                    {
                        if (jet_sprites[i].x >= 256 - 24)
                        {
                            jet_sprites[i].state == OFF_SCREEN;
                            jet_sprites[i].enable = false;
                            jet_sprites[i].jumpers = 0;
                        }
                    }
                }

                if (   (bomb_sprites[i].x <= 0)
                    ||(bomb_sprites[i].x >=256-8)
                    ||(bomb_sprites[i].y >= 192-8))
                {
                    bomb_sprites[i].state = OFF_SCREEN;
                    bomb_sprites[i].enable = false;
                }

                if (hel_sprites[i].state == ON_SCREEN)
                {
                    if (hel_sprites[i].going_left)
                        hel_sprites[i].x += -hel_sprites[i].inc_x;
                    else
                        hel_sprites[i].x += hel_sprites[i].inc_x;
                }

                if (jet_sprites[i].state == ON_SCREEN)
                {
                    if (jet_sprites[i].going_left)
                        jet_sprites[i].x += -jet_sprites[i].inc_x;
                    else
                        jet_sprites[i].x += jet_sprites[i].inc_x;
                }

                if (bomb_sprites[i].state == ON_SCREEN)
                {
                    if (bomb_sprites[i].going_left)
                        bomb_sprites[i].x += -bomb_sprites[i].inc_x;
                    else
                        bomb_sprites[i].x += bomb_sprites[i].inc_x;
                    bomb_sprites[i].y += bomb_sprites[i].inc_y;
                }

                if (shot_sprites[i].state == ON_SCREEN)
                {
                    if (shot_sprites[i].going_left)
                        shot_sprites[i].x += -shot_sprites[i].inc_x;
                    else
                        shot_sprites[i].x += shot_sprites[i].inc_x;
                    shot_sprites[i].y += shot_sprites[i].inc_y;
                }
            } // move helicopters...

        } // while true   
    } // for wave
    end();
    
}
