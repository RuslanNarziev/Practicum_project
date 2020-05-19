#include "sock_wrap.h"
using namespace SQL_Sockets;

int main() {
    const char* address = "mysocket";
    ServerSocket sock;
    try { 
        sock = ServerSocket(address);
        std::cout << 2222 <<std::endl;
    } 
    catch (Exception & err) {
        err.report();
        unlink(address);
    }
    bool end = false;
    while(!end) {
        std::string str;
        str = sock.getstring(sock.getdescr());
        if(str == "END")
            end = true;
        std::string ans = "OK\n";
        sock.putstring(ans, sock.getdescr());
    }
    return 0;
}