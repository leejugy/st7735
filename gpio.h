#ifndef __GPIO__
#define __GPIO__

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <linux/gpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <semaphore.h>

#define GPIO_ROUTE "/dev/gpiochip"

#define printr(fmt, ...) printf("[\x1b[31m" fmt "\x1b[0m]\n", ##__VA_ARGS__)
#define GET_1BITS(bit, shift) (bit >> shift) & 0x01
#define SET_1BITS(val, shift) val | (1 << shift)

typedef enum
{
    GPIO_PIN_A0,    /* chip select when send data (P8_11, line 13, gpio chip 0) */
    GPIO_PIN_RESET, /* chip reset (P8_13, line 23, gpio chip 3) */
    GPIO_PIN_LED,   /* chip led (P8_14, line 26, gpio chip 3) */
    GPIO_PIN_CS,    /* cs pin (p8_11, line 12, gpio chip 0) */
    GPIO_PIN_MAX,
} GPIO_PIN_NAME;

typedef enum
{
    GPIO_LOW,
    GPIO_HIGH,
} GPIO_STATUS;

typedef struct
{
    int chip;
    int line;
    int attribute; /* gpio attribute like GPIO_V2_LINE_FLAG_OUTPUT or GPIO_V2_LINE_FLAG_INPUT*/
    int req_fd;
    sem_t sem;
} gpio_pin_t;

void gpio_init();
int gpio_read(GPIO_PIN_NAME data_index, int *gpio_val);
int gpio_write(GPIO_PIN_NAME data_index, int gpio_val);

#define GPIO_A0_HIGH gpio_write(GPIO_PIN_A0, GPIO_HIGH)       /* not use sci */
#define GPIO_A0_LOW gpio_write(GPIO_PIN_A0, GPIO_LOW)         /* use sci*/
#define GPIO_RESET_HIGH gpio_write(GPIO_PIN_RESET, GPIO_HIGH) /* not reset */
#define GPIO_RESET_LOW gpio_write(GPIO_PIN_RESET, GPIO_LOW)   /* reset */
#define GPIO_LED_HIGH gpio_write(GPIO_PIN_LED, GPIO_HIGH)     /* turn on led */
#define GPIO_LED_LOW gpio_write(GPIO_PIN_LED, GPIO_LOW)       /* turn off led */
#define GPIO_CS_HIGH gpio_write(GPIO_PIN_CS, GPIO_HIGH)       /* chip select off */
#define GPIO_CS_LOW gpio_write(GPIO_PIN_CS, GPIO_LOW)         /* chip select on */
#endif