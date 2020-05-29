#include "sock_wrap.h"
#include "parserSQL.h"

using namespace SQL_Sockets;

int main ()
{
    const char* address = "mysocket";
    ServerSocket socket;
    int client;
/*    catch (Exception & err)
	{
        err.report ();
        unlink (address);
    }*/
    try
	{ 
        socket = ServerSocket (address);
        client = socket.accept ();
        std::string a;
        bool fin = false;
        while(!fin)
		{
            std::string s;
            s = socket.getstring(client);
            Parser P (s.data());
            if (s == "" || s == "EXIT" || s == "EXIT\0" || s == "EXIT\r" || s == "EXIT\n")
			{
                fin = true;
                a = "SUCCESS";
            }
            else
            {
                a = P.parse () + '\n';
            }
            socket.putstring (a, client);
        }
    }
	catch (Exception & err)
	{
        err.report ();
        unlink (address);
    }
    unlink (address);
    return 0;
}