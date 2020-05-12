#include "sock_wrap.h"
using namespace SQL_Sockets;
using namespace Parser;

int main() {
    const char* address = "mysocket"
    std::string comand;
    try {
        ClientSocket sock(address);
        sock.connect();
        bool end = false;
        while(!end) {
            std::cin >> comand;
            if(comand == "end")
                end = true;
            else {
                sock.putstring(comand);
                std::cout << sock.getstring();
            } 
        }
    } 
    catch (Exception & err) {
        err.report();
    }
    return 0;
}