#include "gpio.h"

gpio_pin_t gpio[GPIO_PIN_MAX];

static void gpio_struct_init()
{
    GPIO_PIN_NAME i = 0;
    for (i = 0; i < GPIO_PIN_MAX; i++)
    {
        switch (i)
        {
        case GPIO_PIN_A0:
            gpio[GPIO_PIN_A0].chip = 0;
            gpio[GPIO_PIN_A0].line = 13;
            gpio[GPIO_PIN_A0].attribute = GPIO_V2_LINE_FLAG_OUTPUT;
            break;

        case GPIO_PIN_RESET:
            gpio[GPIO_PIN_RESET].chip = 3;
            gpio[GPIO_PIN_RESET].line = 23;
            gpio[GPIO_PIN_RESET].attribute = GPIO_V2_LINE_FLAG_OUTPUT;
            break;

        case GPIO_PIN_LED:
            gpio[GPIO_PIN_LED].chip = 3;
            gpio[GPIO_PIN_LED].line = 26;
            gpio[GPIO_PIN_LED].attribute = GPIO_V2_LINE_FLAG_OUTPUT;
            break;

        case GPIO_PIN_CS:
            gpio[GPIO_PIN_CS].chip = 0;
            gpio[GPIO_PIN_CS].line = 11;
            gpio[GPIO_PIN_CS].attribute = GPIO_V2_LINE_FLAG_OUTPUT;
            break;
        default:
            break;
        }
    }
}

static int gpio_pin_init(GPIO_PIN_NAME data_index)
{
    int fd = 0;
    int ret = 0;
    char gpio_route[64] = {
        0,
    };
    struct gpio_v2_line_request req;
    memset(&req, 0, sizeof(req));

    sprintf(gpio_route, "%s%d", GPIO_ROUTE, gpio[data_index].chip);
    gpio_struct_init();

    ret = sem_init(&gpio[data_index].sem, 0, 1);
    if (ret < 0)
    {
        perror("fail to init semaphore");
        return -1;
    }

    ret = open(gpio_route, O_RDWR, 0666);
    if (ret < 0)
    {
        printr("gpio index : %d", data_index);
        perror("fail to open");
        return -1;
    }

    fd = ret;
    req.offsets[0] = gpio[data_index].line;
    req.num_lines = 1;
    req.config.num_attrs = 0;
    req.config.flags = gpio[data_index].attribute;

    ret = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req);
    if (ret < 0)
    {
        printr("gpio index : %d", data_index);
        perror("fail get config ioctl");
        close(fd);
        return -1;
    }

    if (req.fd <= 0)
    {
        printr("gpio index : %d", data_index);
        perror("fail get filedescripter req");
        close(fd);
        return -1;
    }

    gpio[data_index].req_fd = req.fd;

    close(fd);
    return 1;
}

void gpio_init()
{
    GPIO_PIN_NAME data_index = 0;
    memset(gpio, 0, sizeof(gpio));

    for (data_index = 0; data_index < GPIO_PIN_MAX; data_index++)
    {
        gpio_pin_init(data_index);
    }
}

int gpio_read(GPIO_PIN_NAME data_index, int *gpio_val)
{
    int fd = 0;
    int ret = 0;
    struct gpio_v2_line_values val;

    fd = gpio[data_index].req_fd;
    memset(&val, 0, sizeof(val));

    val.mask = 1; /* get only one bit */

    //sem_wait(&gpio[data_index].sem);
    ret = ioctl(fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &val);
    //sem_post(&gpio[data_index].sem);

    if (ret < 0)
    {
        printr("gpio index : %d", data_index);
        perror("fail get gpio line info");
        return -1;
    }

    *gpio_val = GET_1BITS(val.bits, 0);
    return 1;
}

int gpio_write(GPIO_PIN_NAME data_index, int gpio_val)
{
    int fd = 0;
    int ret = 0;
    struct gpio_v2_line_values val;

    fd = gpio[data_index].req_fd;
    memset(&val, 0, sizeof(val));

    val.mask = 1; /* get only one bit */
    val.bits = gpio_val;

    //sem_wait(&gpio[data_index].sem);
    ret = ioctl(fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &val);
    //sem_post(&gpio[data_index].sem);

    if (ret < 0)
    {
        printr("gpio index : %d", data_index);
        perror("fail set gpio line info");
        return -1;
    }

    return 1;
}