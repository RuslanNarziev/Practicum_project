#include "sock_wrap.h"
using namespace SQL_Sockets;

ClientSocket::ClientSocket(const char* address) {
    desc = socket (AF_UNIX, SOCK_STREAM, 0);
    if(desc == -1)
        throw Exception(cl_sock);
    addr.sun_family = AF_UNIX;
    strcpy (addr.sun_path, address);
}

void ClientSocket::connect() {
    len = sizeof ( sa.sun_family) + strlen ( sa.sun_path); 
    if( connect (desc, (struct sockaddr *)&sa, len) < 0 ) { 
        throw Exception(cl_con);
    }
}