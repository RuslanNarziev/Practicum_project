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
        bool fin = false;
        while (!fin)
        {
            std::getline (std::cin, comand);
/*            if (comand[0] == 'E')
            {
                std::cout << int(comand[4]);
            }
            std::cout << comand.size() << std::finl;*/
            if (comand == "" || comand == "EXIT" || comand == "EXIT\0" || comand == "EXIT\r" || comand == "EXIT\n")
            {
                fin = true;
            }
            else
                comand += '\n';
            if (!std::cin)
            {
                fin = true;
            }
            if (fin)
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
