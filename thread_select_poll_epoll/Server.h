#ifndef SERVER_H
#define SERVER_H
#include "../config.h"
class Server
{

public:
    Server();
    ~Server();
    void runServer();
    static void * thread_function(void * arg);
    



    int server_fd,ret;
    int client_socket;
    struct sockaddr_in address,remote_address;
    
};


#endif