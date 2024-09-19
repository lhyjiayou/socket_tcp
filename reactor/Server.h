

#ifndef SERVER_H
#define SERVER_H
#include "../config.h"
#define BUf_LENGTH 1024






class Server
{

public:
    Server();
    ~Server();
    void runServer();
    
    

private:
    typedef int (Server::*CALLBACK)(int fd);

    struct conn_item
    {
        int fd;
        char rbuf[BUf_LENGTH];
        int rlen;
        char wbuf[BUf_LENGTH];
        int wlen;
        CALLBACK send_callback;
        union 
        {
            CALLBACK recv_callback;
            CALLBACK accept_callback;
        }handle_in;
        int is_connection;
    };
    struct conn_item connlist[1024]={0};
    int send_cb(int fd);
    int recv_cb(int fd);
    int accept_cb(int fd);
    void set_event(int fd,int event,int flag);
    int initial_server(int port);
    int epfd=-1;
    struct epoll_event events[1024]={0};



    struct sockaddr_in address,remote_address;
    
};


#endif