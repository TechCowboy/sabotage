#include "colorset.h"

unsigned char mode_ii_color_set[2048*3]; //


void set_color_region(char *start, unsigned char pattern, int length)
{
    memset(start, pattern, length);

}