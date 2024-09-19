#include "Client.h"

int main()
{
    Client client;

    //std::thread clientThread(&Client::runClient, &client); 
        // 等待线程完成

    //clientThread.join();
    client.runClient();
    return 1;
    
}