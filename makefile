CC=gcc -g
all: server client
server: server.o
client: client.o
clean:
	rm -f *.o server client