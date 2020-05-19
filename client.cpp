#include "sock_wrap.h"
using namespace SQL_Sockets;

int main() {
    const char* address = "mysocket";
    std::string comand;
    try {
        ClientSocket sock(address);
        sock.connect();
        bool end = false;
        while(!end) {
            std::cin >> comand;
            comand += '\n';
            if(comand == "EXIT")
                end = true;
            sock.putstring(comand, sock.getdescr());
            std::cout << sock.getstring(sock.getdescr());
        }
    } 
    catch (Exception & err) {
        err.report();
    }
    return 0;
}