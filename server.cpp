#include "sock_wrap.h"
using namespace ModelSQL;

int main() {
    const char* address = "mysocket";
    try { 
        ServerSocket sock(address);
    } 
    catch (Exception & err) {
        err.report();
        unlink(address);
    }
    bool end = false;
    while(!end) {
        std::string str;
        str << sock.getstring(sock.getdescr());
        if(str == "END")
            end = true;
        std::string ans;
        sock.putstring(and, sock.getdescr())
    }
    return 0;
}