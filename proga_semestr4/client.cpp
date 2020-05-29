#include "sock_wrap.h"

using namespace SQL_Sockets;

int main ()
{
    const char* address = "mysocket";
    std::string comand;
    try
    {
        ClientSocket sock (address);
        sock.connect ();
        bool end = false;
        while (!end)
        {
            std::getline (std::cin, comand);
/*            if (comand[0] == 'E')
            {
                std::cout << int(comand[4]);
            }
            std::cout << comand.size() << std::endl;*/
            if (comand == "" || comand == "EXIT" || comand == "EXIT\0" || comand == "EXIT\r" || comand == "EXIT\n")
            {
                end = true;
            }
            else
                comand += '\n';
            if (!std::cin)
            {
                end = true;
            }
            if (end)
            {
                comand = "EXIT";
            }
            sock.putstring (comand, sock.getdescr());
            std::cout << sock.getstring (sock.getdescr());
//            std::cout << comand;
        }
    } 
    catch (Exception & err)
    {
        err.report ();
    }
    return 0;
}
