#include "sock_wrap.h"
using namespace SQL_Sockets;

int main() {
    const char* address = "mysocket";
    ServerSocket sock;
    int client;
    try { 
        sock = ServerSocket(address);
        client = sock.accept();
    } 
    catch (Exception & err) {
        err.report();
        unlink(address);
    }
    try {
        bool end = false;
        while(!end) {
            std::string str;
            str = sock.getstring(client);
            std::cout << str ;
            if(str == "END")
                end = true;
            std::string ans = "OK\n";
            sock.putstring(ans, client);
        }
    } catch(Exception & err) {
        err.report();
    }
    return 0;
}