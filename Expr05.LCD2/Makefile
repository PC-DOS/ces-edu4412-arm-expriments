
CROSS_COMPILE=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip
	
all:
	$(CC) -W -o LCD2 LCD2.C
	$(STRIP) LCD2

clean:
	rm -f LCD2
