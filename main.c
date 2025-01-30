#include "gpio.h"
#include "spi.h"
#include "st7735.h"

int main()
{
    start_st7735_thread();

    while(1)
    {
        sleep(1);
    }
}