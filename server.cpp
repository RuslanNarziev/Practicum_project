#include "sock_wrap.h"
#include "parser.h"
using namespace SQL_Sockets;

int main() {
    const char* address = "mysocket";
    ServerSocket sock;
    int client;
    try { 
        sock = ServerSocket(address);
        client = sock.accept();
        bool end = false;
        std::string ans;
        while(!end) {
            std::string str;
            str = sock.getstring(client);
            Parser A(str.data());
            if(str == "EXIT\0") {
                end = true;
                ans = "Work is done";
            } else
                ans = A.parse() + '\n';
            sock.putstring(ans, client);
        }
    } catch(Exception & err) {
        err.report();
        unlink(address);
    }
    unlink(address);
    return 0;
}
