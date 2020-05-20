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
    } 
    catch (Exception & err) {
        err.report();
        unlink(address);
    }
    try {
        bool end = false;
        while(!end) {
            Parser A(str.data());
            std::string str;
            str = sock.getstring(client);
            if(str == "END") {
                end = true;
                ans = "Work is done";
            else
                ans = A.parse();
            sock.putstring(ans, client);
        }
    } catch(Exception & err) {
        err.report();
    }
    return 0;
}