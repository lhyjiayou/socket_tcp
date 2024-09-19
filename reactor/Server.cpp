#include "Server.h"


Server::Server(/* args */)
{

}

Server::~Server()
{
}

int Server::accept_cb(int fd){
    socklen_t addrlen=sizeof(remote_address);
    int client_socket=accept(fd,(sockaddr*)&remote_address,&addrlen);
    set_event(client_socket,EPOLLIN,0);
    std::cout << "client_socket"<<client_socket<< std::endl;
    connlist[client_socket].fd=client_socket;
    memset(connlist[client_socket].rbuf,0,BUf_LENGTH);
    connlist[fd].rlen=0;
    memset(connlist[client_socket].wbuf,0,BUf_LENGTH);
    connlist[client_socket].wlen=0;
    connlist[client_socket].send_callback=&Server::send_cb ;
    connlist[client_socket].handle_in.recv_callback=&Server::recv_cb;
    connlist[client_socket].is_connection=0;
    return client_socket;
}

int Server::recv_cb(int fd){
    int ret=-1;
    int len=connlist[fd].rlen;
    char *buff=connlist[fd].rbuf;
    

    ret=recv(fd,buff+len,BUf_LENGTH-len,0);
    if (ret==0)
    {
        std::cout << fd <<": disconnect"<<std::endl;
        set_event(fd,EPOLL_CTL_DEL,-1);
        close(fd);
        return 0;
    }else if (ret==-1)
    {
        std::cerr<<"wrong"<<std::endl;
        return -1;
    }
    std::cout<<"recv"<< fd <<":"<< connlist[fd].rbuf<<std::endl;
    connlist[fd].rlen+=ret;

    memcpy(connlist[fd].wbuf,connlist[fd].rbuf,connlist[fd].rlen);
    connlist[fd].wlen=connlist[fd].rlen;
    connlist[fd].rlen=0;

    set_event(fd,EPOLLOUT,1);
    return ret;

}

int Server::send_cb(int fd){
    int ret=-1;
    char *buff=connlist[fd].wbuf;
    int len=connlist[fd].wlen;
    ret=send(fd,(void*)buff,len,0);
    if (ret==-1){
        std::cerr<<"wrong"<<std::endl;
        return -1;
    }
    std::cout<<"send"<< fd <<":"<< connlist[fd].wbuf<<std::endl;   
    set_event(fd,EPOLLIN,1);

    return ret;

}

void Server::set_event(int fd,int event,int flag){
    if(flag==-1)
    {
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
    }
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=event;
    if (flag==0)
    {
        epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);

    }else 
    {
        epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
    }

    
}

int Server::initial_server(int port){
    int ret=-1;
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if (fd<0)
    {
        std::cout << "wrong" << std::endl;
        return -1;
    }
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    //address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (inet_pton(AF_INET, "192.168.19.128", &address.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        return -1;
    }
    ret=bind(fd,(sockaddr *)&address,sizeof(address));
    if(ret<0)
    {
        std::cout << "wrong_bind" << std::endl;
        return -1;
    }

    ret=listen(fd,10);

    if(ret<0)
    {
        std::cout << "wrong_listen" << std::endl;
        return -1;
    }
    return fd;
}

void Server::runServer()
{
    int ret=-1;
    int port=8080;
    int port_count=3;
    epfd=epoll_create(1);

    for (int i = 0; i < port_count; i++)
    {
        int server_fd=initial_server(i+port);
        set_event(server_fd,EPOLLIN,0);
        connlist[server_fd].fd=server_fd;
        connlist[server_fd].handle_in.accept_callback=&Server::accept_cb;
        connlist[server_fd].is_connection=1;
    }
    std::cout << "waiting"<< std::endl;

    while (1)
    {
        int nready=epoll_wait(epfd,events,1024,-1);
        for (int i = 0; i < nready; i++)
        {   
            struct epoll_event ev=events[i];
            int connfd=ev.data.fd;
            if (ev.events & EPOLLIN)
            {
                if(connlist[connfd].is_connection){
                    (this->*(connlist[connfd].handle_in.accept_callback))(connfd);
                    
                }else
                {
                    (this->*(connlist[connfd].handle_in.recv_callback))(connfd);
                    //std::cout<< connfd << ":"<<connlist[connfd].rbuf<<std::endl;
                }
                
            }else if (ev.events & EPOLLOUT)
            {
                (this->*(connlist[connfd].send_callback))(connfd);
                //std::cout<< connfd << connlist[connfd].wbuf<<std::endl;           
            }                
        }
    }
    getchar();
    
    return ; 
}

