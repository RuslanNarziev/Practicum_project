#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
const char* address = "mysocket";
int main () {
    char c;
    int i, s, len;
    FILE *fp;
    struct sockaddr_un sa;
    if ((s = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
        std::cout << "client: socket";
    exit (1);
    }
    sa.sun_family = AF_UNIX;
    strcpy (sa.sun_path, address);
    len = sizeof ( sa.sun_family) + strlen ( sa.sun_path); 
    if ( connect ( s, (struct sockaddr *)&sa, len) < 0 ) { 
        std::cout << "client: connect";
        exit (1);
    }
    fp = fdopen (s, "r"); 
    send (s, "client", 7, 0);
    close (s); 
    exit (0);
}