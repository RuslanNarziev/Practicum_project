#include "sock_wrap.h"

using namespace SQL_Sockets;

ClientSocket::ClientSocket (const char* address)
{
    desc = socket (AF_UNIX, SOCK_STREAM, 0);
    if (desc == -1)
    {
        throw Exception (CL_SOCK);
	}
	addr.sun_family = AF_UNIX;
    strcpy (addr.sun_path, address);
}

void ClientSocket::connect ()
{
    int len = sizeof (addr.sun_family) + strlen ( addr.sun_path); 
    if (::connect(desc, (struct sockaddr *)&addr, len) < 0 ) 
	{ 
        throw Exception (CL_CON);
    }
}

void BaseSocket::putstring (std::string & str, int d)
{
    if (send (d, str.data (), strlen (str.data ()) + 1, 0) == -1)
        throw Exception (Exception(SEND));
}

char BaseSocket::getchar (int d)
{
    char c = 0;
    if (recv (d, &c, sizeof(char), 0) == -1)
        throw Exception (Exception(RECV));
    return c;
}

std::string BaseSocket::getstring (int d)
{
    char c = 0;
    std::string answer;
    c = getchar (d);
    while (c != '\0')
	{
        answer += c;
        c = getchar (d);
    }
    return answer;
}

int BaseSocket::getdescr ()
{
    return desc;
}

ServerSocket::ServerSocket (const char* address)
{
    struct sockaddr_un sa;
    if ((desc = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
        throw Exception (SV_SOCK);
    sa.sun_family = AF_UNIX;
    strcpy (sa.sun_path, address);
    unlink (address);
    int len = sizeof ( sa.sun_family) + strlen (sa.sun_path);
    if (bind (desc, (struct sockaddr *)&sa, len) < 0 )
        throw Exception (SV_BIND);
    if (listen (desc, 5) < 0 )
        throw Exception (SV_LIST);
}

int ServerSocket::accept ()
{
    int d_cl;
    if ((d_cl = ::accept (desc, 0, 0)) < 0)
        throw Exception (SV_ACC);
    return d_cl;
}

void Exception::report ()
{
    std::cout << type;
}