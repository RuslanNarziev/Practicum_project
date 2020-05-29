CC = g++

all: client server

server: sock_wrap.o parser.o table.o
	$(CC) -o server sock_wrap.o table.o parser.o server.cpp

client: sock_wrap.o
	$(CC) -o client sock_wrap.o client.cpp

parser.o: table.o
	$(CC) -c parser.cpp parser.h

table.o:
	$(CC) -c table.cpp table.h

sock_wrap.o:
	$(CC) -c sock_wrap.cpp sock_wrap.h

clean:
	rm -rf *.o
	rm -rf *.h.gch
