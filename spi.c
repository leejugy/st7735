#include "spi.h"
spi_sturct_t spi[SPIDEV_MAX];
sem_t spi_sem[SPI_BUS_NUM];

static void spi_init_struct(SPI_INDEX data_index)
{
    int ret = 0;
    uint8_t i = 0;

    switch (data_index)
    {
    case SPIDEV0_0:
    case SPIDEV0_1:
        spi[data_index].bit = 8;
        spi[data_index].lsb = 0;
        spi[data_index].mode = SPI_MODE_0; /* 첫번째 엣지에서 샘플림, idle시 low */
        spi[data_index].speed = 1000000;
        spi[data_index].sem = &spi_sem[0];
        break;

    case SPIDEV1_0:
    case SPIDEV1_1:
        spi[data_index].bit = 8;
        spi[data_index].lsb = 0;
        spi[data_index].mode = SPI_MODE_0;
        spi[data_index].speed = 1000000;
        spi[data_index].sem = &spi_sem[1];
        break;

    default:
        break;
    }

    for(i = 0 ; i < SPI_BUS_NUM; i++)
    {
        ret = sem_init(&spi_sem[i], 0, 1);
        if(ret < 0)
        {
            perror("fail to init sem");
        }
    }

    switch (data_index)
    {
    case SPIDEV0_0:
        spi[data_index].spi_route = SPI_ROUTE "0.0";
        break;

    case SPIDEV0_1:
        spi[data_index].spi_route = SPI_ROUTE "0.1";
        break;

    case SPIDEV1_0:
        spi[data_index].spi_route = SPI_ROUTE "1.0";
        break;

    case SPIDEV1_1:
        spi[data_index].spi_route = SPI_ROUTE "1.1";
        break;

    default:
        break;
    }
}

int spi_init(SPI_INDEX data_index)
{
    int ret = 0;
    int fd = 0;

    spi_init_struct(data_index);

    ret = open(spi[data_index].spi_route, O_RDWR, 0666);
    if (ret < 0)
    {
        perror("fail to open spi route");
        return -1;
    }

    fd = ret;
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi[data_index].bit);
    if (ret < 0)
    {
        perror("fail to set spi bit");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &spi[data_index].lsb);
    if (ret < 0)
    {
        perror("fail to set spi lsb");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi[data_index].speed);
    if (ret < 0)
    {
        perror("fail to set spi max speed");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_WR_MODE, &spi[data_index].mode);
    if (ret < 0)
    {
        perror("fail to set spi mode");
        return -1;
    }

    spi[data_index].fd = fd;
    return 1;
}

int spi_half_duplex_write(SPI_INDEX data_index, void *buffer, size_t buffer_size)
{
    int ret = 0;

    sem_wait(spi[data_index].sem);
    ret = write(spi[data_index].fd, buffer, buffer_size);
    if (ret < 0)
    {
        perror("fail to write spi");
    }
    sem_post(spi[data_index].sem);
    return ret;
}

int spi_half_duplex_read(SPI_INDEX data_index, void *buffer, size_t buffer_size)
{
    int ret = 0;

    sem_wait(spi[data_index].sem);
    ret = write(spi[data_index].fd, buffer, buffer_size);
    if (ret < 0)
    {
        perror("fail to read spi");
    }
    sem_post(spi[data_index].sem);
    return ret;
}

int spi_full_duplex_read_write(SPI_INDEX data_index, void *tx_buffer, void *rx_buffer, size_t buffer_size)
{
    struct spi_ioc_transfer spi_buf;
    int ret = 0;
    memset(&spi_buf, 0, sizeof(spi_buf));
    spi_buf.tx_buf = (uint32_t)tx_buffer;
    spi_buf.rx_buf = (uint32_t)rx_buffer;
    spi_buf.len = buffer_size;
    
    sem_wait(spi[data_index].sem);
    ret = ioctl(spi[data_index].fd, SPI_IOC_MESSAGE(1), &spi_buf);
    sem_post(spi[data_index].sem);
    
    if(ret < 0)
    {
        perror("fail to spi full duplex");
    }
}
