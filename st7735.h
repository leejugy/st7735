#ifndef __ST7735__
#define __ST7735__

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}st7735_rgb_t;

typedef enum
{
    ST7735_COLOR_RED,
    ST7735_COLOR_GREEN,
    ST7735_COLOR_BLUE,
}RGB_COLOR;

void start_st7735_thread();
#endif