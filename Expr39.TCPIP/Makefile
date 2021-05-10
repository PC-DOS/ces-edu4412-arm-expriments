
CC=$gcc
STRIP=$(CROSS_COMPILE)strip

all:
	$(CC) -W -o server  server.c 
	$(STRIP) server
	$(CC) -W -o client  client.c 
	$(STRIP) client

clean:
	rm -f server client 
