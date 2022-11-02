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
    int run_count = 0;
    int stop_at = 168;
    int display_debug = false;
    char temp[80];
    int  answer;
    bool steerable = false;
    int right_start_x = 0;
    int left_start_x = 256 - 8 * 3;
    int direction = 1;
    int n;
    int start, stop, inc, wave_done, sprite_collision, total_sprites;
    bool left = false;
    int fire;
    char c;
    SPRITE_STATE all_sprites[32];
    int shot_delay = 30;
    WAVE waves[] = 
    {
        { 2,  // Helicopters
         24,  // Heli-height
         CHOPTER_SLOW, // speed
          1,  // jumpers per helicopter
          0,  // jets
          0}, // bombs per jet
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

    //sound_init();
    //init_keyboard();
    
    for(int i=0; i<32; i++)
    {
        all_sprites[i].flip = rand() < 16384 ? true : false;
        all_sprites[i].enable = false;
        all_sprites[i].state = OFF_SCREEN;
        all_sprites[i].jumpers = 1;
        all_sprites[i].going_left = left;
        all_sprites[i].appearance_wait = my_rand(8, 256 - 8);
        all_sprites[i].disappearance_wait = 0;
        all_sprites[i].y = SPRITE_TERMINATOR;
        all_sprites[i].inc_x = CHOPTER_SLOW;
        all_sprites[i].inc_y = 0;

        left = ! left;
    }

    answer = introduction();
    steerable = ((answer == 'Y') || (answer == 'y'));


    
    mode_graphics_ii();
    clr(' ');
    test_char_color();
    getchar();

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
            all_sprites[i].sprite_type = SPRITE_TYPE_NONE;
            all_sprites[i].disappearance_wait = 0;
        }

        total_sprites = 0;
        for (int i = 0; i < waves[wave].helicopters; i++)
        {
            all_sprites[total_sprites].sprite_type = SPRITE_TYPE_HELICOPTER;
            all_sprites[total_sprites].enable = true;
            all_sprites[total_sprites].state = OFF_SCREEN;
            all_sprites[total_sprites].jumpers = waves[wave].jumpers;
            all_sprites[total_sprites].inc_x = waves[wave].heli_speed;
            all_sprites[total_sprites].inc_y = 0;

            all_sprites[total_sprites].going_left = left;

            if (all_sprites[total_sprites].going_left)
                all_sprites[total_sprites].x = left_start_x;
            else
                all_sprites[total_sprites].x = right_start_x;

            all_sprites[total_sprites].y = set_y(hel_pos);
            all_sprites[total_sprites].appearance_wait = (total_sprites > 0) ? my_rand(8, 80) : 5;
            all_sprites[total_sprites].jump_wait = my_rand(32, 256 - 32);
            hel_pos += 16;

            total_sprites++;
            left = !left;
        }

        int jet_pos = 0;

        left = my_rand(0,1);
        for (int i = 0; i < waves[wave].jets; i++)
        {
            all_sprites[total_sprites].sprite_type = SPRITE_TYPE_JET;
            all_sprites[total_sprites].enable = true;
            all_sprites[total_sprites].state = OFF_SCREEN;
            all_sprites[total_sprites].bombs = waves[wave].bombs_per_jet;
            all_sprites[total_sprites].inc_x = JET_SPEED;
            all_sprites[total_sprites].inc_y = 0;

            all_sprites[total_sprites].going_left = left;

            if (all_sprites[total_sprites].going_left)
                all_sprites[total_sprites].x = left_start_x;
            else
                all_sprites[total_sprites].x = right_start_x;

            all_sprites[total_sprites].y = set_y(jet_pos);
            all_sprites[total_sprites].appearance_wait = my_rand(8, 80);
            all_sprites[total_sprites].bomb_wait = my_rand(32, 64) / JET_SPEED;
            jet_pos += 16;
            total_sprites++;

            left = !left;
        }

        
        while(true)
        {
            total_sprites = 0;
            run_count++;
            if (display_debug)
            {
                sprintf(title, "Run Count %d", run_count);
                vprint(title, 2);
            }

            rotation = read_keyboard(rotation);
            fire = (rotation & 16384);

            rotation = read_joystick(rotation);
            fire = (rotation & 16384) | fire; // joystick or keyboard

            rotation = rotation & ~16384;
            create_text_turret(rotation);

            remove_sprites();
            
            wave_done = true;
            for(int i=0; i <32; i++)
            {
                // ***************************
                // add helicopters
                // ***************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_HELICOPTER)
                {
                    if (all_sprites[i].enable)
                    {
                        wave_done = false;
                       
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            n = helicopter(total_sprites, &all_sprites[i]);

                            total_sprites += n;
                            if (total_sprites >= 31)
                            {
                                total_sprites -= n;
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

                // ***************************
                // add jets
                // ***************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_JET)
                {
                    if (all_sprites[i].enable)
                    {
                        wave_done = false;
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            n = jet(total_sprites, &all_sprites[i]);

                            total_sprites += n;
                            if (total_sprites >= 31)
                            {
                                total_sprites -= n;
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

                // ***************************
                // add men / chutes
                // ***************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_MAN)
                {
                    /* ADD MAN SPRITES */
                    if (all_sprites[i].enable)
                    {
                        wave_done = false;
                        n = parachuter(total_sprites, &all_sprites[i]);
                        total_sprites += n;
                        if (total_sprites >= 31)
                        {
                            total_sprites -= n;
                            sprintf(title, "TOO MANY SPRITES!");
                            vprint(title, 13);
                            break;
                        }

                    } // if enable
                } // if man

                // ***************************
                // add shots
                // ***************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_SHOT)
                {
                    if (all_sprites[i].enable)
                    {
                        /* ADD SHOT SPRITES */
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            wave_done = false;
                            // sprintf(title, "SHOT! (%d,%d) sprite %d", all_sprites[i].x, all_sprites[i].y, sprite);
                            // vprint(title, 4);
                            n = shot(total_sprites, &all_sprites[i]);

                            // emergency_stop = 1;
                            total_sprites += n;
                            if (total_sprites >= 31)
                            {
                                total_sprites -= n;
                                sprintf(title, "TOO MANY SPRITES!");
                                vprint(title, 13);
                                break;
                            }
                        } // if on screen

                    } // if enable
                } // if shot

                // ***************************
                // add bombs
                // ***************************
                if (all_sprites[i].sprite_type == SPRITE_TYPE_BOMB)
                {
                    /* ADD BOMB SPRITES */
                    if (all_sprites[i].enable)
                    {
                        if (all_sprites[i].state == ON_SCREEN)
                        {
                            wave_done = false;
                            n = bomb(total_sprites, &all_sprites[i]);

                            total_sprites += n;
                            if (total_sprites >= 31)
                            {
                                total_sprites -= n;

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
            reverse_sprites();
            vwrite(reverse_attributes, VRAM_SPRITE_ATTRIBUTES, 32 * sizeof(SPRITE_ATTRIBUTE));


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
                                int char_x;
                                
                                char_x = (int)((all_sprites[i].x) / 8) * 8 ;

                                all_sprites[man].sprite_type = SPRITE_TYPE_MAN;
                                all_sprites[man].enable = true;
                                // man needs to fall on a character boundary
                                all_sprites[man].x = char_x - 4;
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
                                all_sprites[bomb].inc_x = all_sprites[i].inc_x > 0 ? 1 : -1;
                                all_sprites[bomb].inc_y = 3;
                                all_sprites[bomb].state = DROPPED;
                            }
                        } // if add_bomb
                    } // if on screen and bomb
                } // if jet
            } // for

            add_sprite = 0;

            if (display_debug)
                if (run_count >= stop_at)
                {
                    vprint("Checking collision", 6);
                }

            // ******************************************************
            // *** COLLISION DETECTION
            // ******************************************************
            for (int i=0; i<32; i++)
            {
                if (display_debug)
                {
                    if (run_count >= stop_at)
                    {
                        char temp[80];
                        identify_sprite(all_sprites[i].sprite_type, temp);
                        sprintf(title, "i:%02d %s", i, temp);
                        vprint(title, 3);
                    }
                }
                if (all_sprites[i].sprite_type != SPRITE_TYPE_SHOT)
                {
                    continue;
                }
                if (display_debug)
                    if (run_count >= stop_at)
                        vprint("shot", 4);

                if (all_sprites[i].enable)
                {
                    if (display_debug)
                        if (run_count >= stop_at)
                            vprint("enable", 4);

                    if (all_sprites[i].state == ON_SCREEN)
                    {
                        if (display_debug)
                            if (run_count >= stop_at)
                                vprint("on screen", 5);

                        sprite_collision = collision_detect(i, all_sprites, display_debug && (run_count >= stop_at));
                        if (sprite_collision != NO_COLLISION)
                        {
                            char temp[80];
                            identify_sprite(all_sprites[sprite_collision].sprite_type, temp);

                            if (display_debug)
                            {
                                sprintf(title, "COLLISION WITH %d %s", sprite_collision, temp);
                                vprint(title, 6);
                                mygetchar();
                            }
                        } 
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

                    if (all_sprites[i].y > BOTTOM_SCREEN_Y - 16 - 2)
                    {
                        all_sprites[i].enable = false;
                        all_sprites[i].state = OFF_SCREEN;
                        int char_x;
                        int char_y;
                        all_sprites[i].y = set_y(all_sprites[i].y + 8);

                        // replace the man sprite with a man character
                        char_x = all_sprites[i].x / 8 + 1;
                        char_y = total_rows - 1;
                        vpoke(VRAM_NAME_TABLE + char_y * total_columns + char_x, CHAR_MAN);
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
                            all_sprites[i].inc_y += 1;
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
                        all_sprites[shot].sprite_type = SPRITE_TYPE_SHOT;
                        all_sprites[shot].enable = true;
                        all_sprites[shot].x = shot_angle[rotation].x;
                        all_sprites[shot].y = shot_angle[rotation].y;
                        all_sprites[shot].inc_x = shot_angle[rotation].inc_x;
                        all_sprites[shot].inc_y = shot_angle[rotation].inc_y;
                        all_sprites[shot].state = ON_SCREEN;
                        //sprintf(title, "%02d shot", shot);
                        //vprint(title, 2);
                    }
                    fire = 0;
                } // if fire
               
            } // for

        } // while true   
    } // for wave
    end_game();
    
}
