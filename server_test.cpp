#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
const char* address = "mysocket";
int main()  {
    char c; 
    int i, d, d1, len;
    unsigned int ca_len;
    FILE * fp;
    struct sockaddr_un sa, ca;
    if((d = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) { 
        std::cout << "server: socket";
        exit (1);
    }
    sa.sun_family = AF_UNIX;
    strcpy (sa.sun_path, address);
    unlink (address);
    len = sizeof ( sa.sun_family) + strlen (sa.sun_path);
    if ( bind ( d, (struct sockaddr *)&sa, len) < 0 ) {
        std::cout << "server: bind";
        exit (1);
    }
    if ( listen ( d, 5) < 0 ) {
        std::cout << "server: listen"; 
        exit (1); 
    }
    ca_len = sizeof ca;

    if (( d1 = accept ( d, (struct sockaddr *)&ca, &ca_len)) < 0 ) {
        std::cout << "server: accept";
        exit (1);
    }
    //fp = fdopen (d1, "r");
    char buf[10];
    recv(d1, buf, 7, 0);
    std::cout << buf;
    close (d1);
    exit (0);
}