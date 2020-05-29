CC = g++
all: server client

server: sock_wrap.o parser.o table.o
	$(CC) -fsanitize=address,undefined -o server sock_wrap.o table.o parser.o server.cpp

client: sock_wrap.o
	$(CC) -fsanitize=address,undefined -o client sock_wrap.o client.cpp

parser.o: table.o
	$(CC) -c parser.cpp parserSQL.h

table.o:
	$(CC) -c table.cpp tableSQL.h

sock_wrap.o:
	$(CC) -c sock_wrap.cpp sock_wrap.h

clean:
	rm -rf *.o
	rm -rf *.h.gch