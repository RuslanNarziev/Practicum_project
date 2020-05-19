#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
namespace SQL_Sockets {
    enum exc_type {CL_SOCK, CL_CON, SEND, RECV, SV_SOCK, SV_BIND, SV_LIST, SV_ACC};
    class Exception { 
        enum exc_type type;
    public: 
        Exception(enum exc_type _type) : type(_type) {} 
        void report();
    };

    class BaseSocket {
    protected:
        int desc;
        struct sockaddr_un addr;
        int getchar(int d);
    public:
        void putstring(std::string& s, int d);
        std::string getstring(int d);
        int getdescr();
    };

    class ClientSocket: public BaseSocket {
    public: 
        ClientSocket(const char* address);
        void connect();
    };

    class ServerSocket: public BaseSocket {
        int d_cl;
    public:
        BaseSocket * Accept();
        ServerSocket(const char * Address);
        ServerSocket() {}
    };
}
#endif