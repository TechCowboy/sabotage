#ifndef _MISC_H
#define _MISC_H

#define MIN(x, y) x < y ? x : y
#define MAX(x, y) x > y ? x : y

#define my_rand(min, max) (rand() % (max - min + 1)) + min

#endif