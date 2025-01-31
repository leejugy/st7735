#include "st7735.h"
#include "gpio.h"
#include "spi.h"

static void st7735_send_cmd(uint8_t cmd)
{
    GPIO_A0_LOW;
    GPIO_CS_LOW;
    spi_half_duplex_write(SPIDEV0_0, &cmd, sizeof(cmd));
    GPIO_CS_HIGH;
    GPIO_A0_LOW;
}

static void st7735_send_data(uint8_t cmd, uint8_t *buffer, size_t buffer_size, uint32_t repeat)
{
    uint32_t i = 0;
    GPIO_A0_LOW;
    GPIO_CS_LOW;
    spi_half_duplex_write(SPIDEV0_0, &cmd, sizeof(cmd));
    GPIO_A0_HIGH;
    for (i = 0; i < repeat; i++)
    {
        spi_half_duplex_write(SPIDEV0_0, buffer, buffer_size);
    }
    GPIO_CS_HIGH;
    GPIO_A0_LOW;
}

static void st7735_sleep_out()
{
    st7735_send_cmd(0x11);
}

static void st7735_display_on()
{
    st7735_send_cmd(0x29);
}

static void st7735_display_off()
{
    st7735_send_cmd(0x28);
}

static void st7735_set_idle_on()
{
    st7735_send_cmd(0x39);
}

static void st7735_set_idle_off()
{
    st7735_send_cmd(0x38);
}

static void st7735_color_mode_set_18bit()
{
    uint8_t tx_buffer = 0;

    tx_buffer = 0x06;
    st7735_send_data(0x3a, &tx_buffer, sizeof(tx_buffer), 1);
}

static void st7735_ram_address_set()
{
    uint8_t tx_buffer = 0;

    tx_buffer = 0x08;
    st7735_send_data(0x36, &tx_buffer, sizeof(tx_buffer), 1);
}

static void st7735_column_set(uint16_t x_start, uint16_t x_end)
{
    uint8_t tx_buffer[4] = {
        0,
    };

    tx_buffer[0] = (x_start >> 8) & 0xff;
    tx_buffer[1] = x_start & 0xff;
    tx_buffer[2] = (x_end >> 8) & 0xff;
    tx_buffer[3] = x_end & 0xff;
    st7735_send_data(0x2a, tx_buffer, sizeof(tx_buffer), 1);
}

static void st7735_raw_set(uint16_t y_start, uint16_t y_end)
{
    uint8_t tx_buffer[4] = {
        0,
    };

    tx_buffer[0] = (y_start >> 8) & 0xff;
    tx_buffer[1] = y_start & 0xff;
    tx_buffer[2] = (y_end >> 8) & 0xff;
    tx_buffer[3] = y_end & 0xff;
    st7735_send_data(0x2b, tx_buffer, sizeof(tx_buffer), 1);
}

static void st7735_write_color(uint8_t red, uint8_t green, uint8_t blue, uint16_t x_len, uint16_t y_len)
{
    uint8_t tx_buffer[3] = {
        0,
    };

    tx_buffer[0] = red;
    tx_buffer[1] = blue;
    tx_buffer[2] = green;
    st7735_send_data(0x2c, tx_buffer, sizeof(tx_buffer), x_len * y_len);
}

static void st7735_init()
{
    spi_init(SPIDEV0_0);
    gpio_init();
    GPIO_LED_HIGH;
    GPIO_RESET_LOW;
    GPIO_A0_LOW;
    GPIO_CS_HIGH;
    usleep(500 * 1000);
    GPIO_RESET_HIGH;

    st7735_sleep_out();
    usleep(10 * 1000);
    st7735_display_on();
    usleep(10 * 1000);
    st7735_color_mode_set_18bit();
    usleep(10 * 1000);
    st7735_ram_address_set();
}

static void st7735_fill_all_screen(st7735_rgb_t *rgb_val)
{
    usleep(10 * 1000);
    st7735_column_set(0, 128);
    usleep(10 * 1000);
    st7735_raw_set(0, 128);
    usleep(10 * 1000);
    st7735_write_color(rgb_val->red, rgb_val->blue, rgb_val->green, 128, 128);
}

static void get_rgb_color(st7735_rgb_t *rgb_val, RGB_COLOR color)
{
    switch (color)
    {
    case ST7735_COLOR_BLUE:
        rgb_val->red = 0;
        rgb_val->green = 0;
        rgb_val->blue = 0xfc;
        break;

    case ST7735_COLOR_RED:
        rgb_val->red = 0xfc;
        rgb_val->green = 0;
        rgb_val->blue = 0;
        break;

    case ST7735_COLOR_GREEN:
        rgb_val->red = 0;
        rgb_val->green = 0xfc;
        rgb_val->blue = 0;
        break;

    default:
        break;
    }
}

static void st7735_thread()
{
    st7735_rgb_t rgb;
    memset(&rgb, 0, sizeof(rgb));
    while (1)
    {
        get_rgb_color(&rgb, ST7735_COLOR_RED);
        st7735_fill_all_screen(&rgb);
        sleep(1);

        get_rgb_color(&rgb, ST7735_COLOR_GREEN);
        st7735_fill_all_screen(&rgb);
        sleep(1);

        get_rgb_color(&rgb, ST7735_COLOR_BLUE);
        st7735_fill_all_screen(&rgb);
        sleep(1);
    }
}

void start_st7735_thread()
{
    pthread_t pid;
    int ret = 0;

    st7735_init();
    ret = pthread_create(&pid, NULL, (void *)&st7735_thread, NULL);
    if (ret < 0)
    {
        perror("fail to creat thread");
    }

    ret = pthread_detach(pid);
    if (ret < 0)
    {
        perror("fail to detach");
    }
}