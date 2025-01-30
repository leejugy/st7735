set -e

export CC="arm-linux-gnueabihf-gcc"
export LDFLAGS="-lrt -pthread"
export CFLAGS="-Wunused"
export FILES="main.c gpio.c spi.c st7735.c"
export BIN_NAME="st7735"

make -j
cp st7735 /mnt/c/linux_share/.