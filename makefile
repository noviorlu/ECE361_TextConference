CC=gcc -g
all: server deliver
server: server.o
client: client.o
clean:
	rm -f *.o server client