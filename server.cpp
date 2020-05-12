#include "sock_wrap.h"
using namespace ModelSQL;

const char* address = "mysocket";
class MyServerSocket : public UnServerSocket {
public: MyServerSocket () : UnServerSocket (address) {}
protected:
 void OnAccept (BaseSocket * pConn)
 {
 // установлено соединение с клиентом, читаем сообщение
 cout << "Read from client: " << pConn->GetString() << endl; // отправляем ответ
 pConn->PutString("Hello from server.");
 // продолжаем диалог с клиентом, пока в этом есть необходимость delete pConn;
 }
};
int main(int argc, char* argv[]) {
    try { 
        ServerSocket sock(address);
        // слушаем запросы на соединение
        for (;;)
        sock.Accept();
    } 
    catch (Exception & err) {
        err.Report();
    }
    return 0;
}