#ifndef __SPI__
#define __SPI__

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <semaphore.h>

#define SPI_ROUTE "/dev/spidev"
#define SPI_BUS_NUM 2
typedef enum
{
    SPIDEV0_0,
    SPIDEV0_1,
    SPIDEV1_0,
    SPIDEV1_1,
    SPIDEV_MAX,
}SPI_INDEX;

typedef enum
{
    SPI_CS0,
    SPI_CS1,
}SPI_CS;

typedef struct
{
    uint8_t mode;
    uint8_t bit;
    uint8_t lsb;
    int fd;
    uint32_t speed;
    char *spi_route;
    sem_t *sem;
}spi_sturct_t;

int spi_init(SPI_INDEX data_index);
int spi_half_duplex_write(SPI_INDEX data_index, void *buffer, size_t buffer_size);
int spi_half_duplex_read(SPI_INDEX data_index, void *buffer, size_t buffer_size);
int spi_full_duplex_read_write(SPI_INDEX data_index, void *tx_buffer, void *rx_buffer, size_t buffer_size);
#endif