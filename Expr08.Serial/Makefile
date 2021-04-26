
CROSS_COMPILE=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip

all:
	$(CC) -W -o serial serial.c
	$(STRIP) serial

clean:
	rm -f serial
