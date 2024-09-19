
#include "Server.h"

int main()
{

    Server server;
    //std::thread serverThread(&Server::runServer, &server);

        // 等待线程完成

    server.runServer();
    

    return 1;
}