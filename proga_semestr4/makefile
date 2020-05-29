CC = g++

all: client server

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
	
parser_fuzzer: parser_fuzzer.cpp table.cpp parser.cpp
	clang++ -g -Wall -fsanitize=address,undefined,fuzzer parser_fuzzer.cpp table.cpp parser.cpp -fprofile-instr-generate -fcoverage-mapping -o parser_fuzzer

clean:
	rm -rf *.o test
	rm -rf *.h.gch