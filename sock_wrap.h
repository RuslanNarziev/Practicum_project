#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__
#include <string>
#include <iostream>
#include <io.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
namespace SQL_Sockets {
    enum exc_type {cl_sock, cl_con};
    class Exception { 
        enum exc_type type;
    public: 
        Exception (enum exc_type _type) : type(_type) {} 
        void report ();
        std::string GetMessage();
    };

    class BaseSocket {
    protected:
        int desc;
        struct sockaddr_un addr;
    public:
        //explicit BaseSocket (int sd = −1, SocketAddress * pAddr = NULL): m_Socket(sd), m_pAddr(pAddr) {}
        virtual ~ BaseSocket();
        void Write(void * buf, int len);
        void putstring(const std::string& s)
        int Read (void * buf, int len);
        std::string getstring();
        int GetSockDescriptor();
    };
    // ClientSocket --- базовый класс для клиентских сокетов
    class ClientSocket: public BaseSocket {
        const char
    public: 
        ClientSocket(const char* address);
        void connect();
    };
    // ServerSocket --- базовый класс для серверных сокетов
    class ServerSocket: public BaseSocket {
    public:
        BaseSocket * Accept();
    protected:
        ServerSocket(const char * Address);
        void bind();
        void listen(int BackLog);
        virtual void OnAccept (BaseSocket * pConn) {}
    };

}; // конец namespace ModelSQL
#endif