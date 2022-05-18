#ifndef _VDP_REGISTERS_H
#define _VDP_REGISTERS_H

typedef union _TMS_REGISTER_0
{
    struct
    {
        unsigned char enable_VDP_input : 1; // Bit 0
        unsigned char M3 : 1;               // Bit 1
        unsigned char unused : 6;           // Bit 2,3,4,5,6,7
    };
    unsigned char reg0;
} TMS_REGISTER_0;

typedef union _TMS_REGISTER_1
{
    struct
    {
        unsigned char Magnify_sprite : 1;      // 0
        unsigned char Sprite_16x16_enable : 1; // 1
        unsigned char reserved : 1;            // 2
        unsigned char M2 : 1;                  // 3
        unsigned char M1 : 1;                  // 4
        unsigned char Interrupt_enable : 1;    // 5
        unsigned char Show_screen : 1;         // 6
        unsigned char RAM_16K : 1;             // 7
    };
    unsigned char reg1;
} TMS_REGISTER_1;

typedef union _TMS_REGISTER_2
{
    struct
    {
        // R2 * 0x400 = Base address of name table
        unsigned char Base_address_of_name_table;
    };
    unsigned char reg2;
} TMS_REGISTER_2;

typedef union _TMS_REGISTER_3
{
    struct
    {
        // R3 * 0x40 = Base address of color table
        unsigned char Base_address_of_color_table;
    };
    unsigned char reg3;
} TMS_REGISTER_3;

typedef union _TMS_REGISTER_4
{
    struct
    {
        // R4 * 0x800 = Base address of generator table
        unsigned char Base_address_of_generator_table;
    };
    unsigned char reg4;
} TMS_REGISTER_4;

typedef union _TMS_REGISTER_5
{
    struct
    {
        // R5 * 0x80 = Base address sprite attribute table
        unsigned char Base_address_of_sprite_attribute_table;
    };
    unsigned char reg5;
} TMS_REGISTER_5;

typedef union _TMS_REGISTER_6
{
    struct
    {
        // R6 * 0x800 = Base address sprite generator table
        unsigned char Base_address_of_sprite_generator_table;
    };
    unsigned char reg6;
} TMS_REGISTER_6;

typedef union _TMS_REGISTER_7
{
    struct
    {
        unsigned char background_color : 4; // background
        unsigned char foreground_color : 4; // 7,6,5,4
    };
    unsigned char reg7;
} TMS_REGISTER_7;

typedef union _TMS_STATUS
{
    struct 
    {
        unsigned char interrupt_flag   : 1;
        unsigned char coincidence_flag : 1;
        unsigned char fifth_sprite     : 5
    };
    unsigned char status;
    
} TMS_STATUS;

// Registers 0 through 7 are Write Only
#define MASK_R0_M3                     0b00000010
#define MASK_R0_VDP_ENABLE             0b00000001

#define MASK_R1_16K                    0b10000000
#define MASK_R1_BLANK                  0b01000000 // Clear bit to blank screen
#define MASK_R1_INTERRUPT              0b00100000
#define MASK_R1_M1                     0b00010000
#define MASK_R1_M2                     0b00001000
#define MASK_R1_SPRITE_SIZE            0b00000010
#define MASK_R1_SPRITE_MAGNIFY         0b00000001

// Only status is read only
#define MASK_VSTATUS_VERTICAL_BLANK    0b10000000
#define MASK_VSTATUS_SPRITE_COLLIDE    0b01000000
#define MASK_VSTATUS_FIFTH_SPRITE_FLAG 0b00100000
#define MASK_VSTATUS_FIFTH_SPRITE_NUM  0b00011111

#endif
