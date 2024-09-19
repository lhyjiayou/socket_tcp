#include "Server.h"


Server::Server(/* args */)
{

}

Server::~Server()
{
}
void * Server::thread_function(void * arg){
    int client_socket=*(int *)arg;
    while (1)
    {
        char buffer[1024]={0};
        int ret=-1;
        
        memset(buffer, 0, sizeof(buffer));
        ret=recv(client_socket,(void*) buffer,1024,0);
        //std::cout<< ret<<client_socket<<std::endl;
        if (ret==0)
        {
            break;
        }
        else if (ret>0)
        {    
        std::cout<< "receive:"<<buffer<<std::endl;
        send(client_socket,(void *)buffer,ret,0);
        }
    }
    close(client_socket);
    return  NULL;

}
void Server::runServer()
{

    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if (server_fd<0)
    {
        std::cout << "wrong" << std::endl;
        return ;
    }
    address.sin_family=AF_INET;
    address.sin_port=htons(8080);
    //address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (inet_pton(AF_INET, "192.168.19.128", &address.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        return ;
    }
    ret=bind(server_fd,(sockaddr *)&address,sizeof(address));
    if(ret<0)
    {
        std::cout << "wrong_bind" << std::endl;
        return;
    }

     ret=listen(server_fd,10);
    
    if(ret<0)
    {
        std::cout << "wrong_listen" << std::endl;
        return;
    }

    while (1)
    {
        std::cout << "waiting"<< std::endl;


    #if 0//多线程方案
        
        socklen_t addrlen = sizeof(remote_address);
        client_socket = accept(server_fd, (struct sockaddr *)&remote_address, &addrlen);
        //client_socket=accept(server_fd,(struct sockaddr *)&remote_address,(socklen_t *)1024); 错误
        std::cout<< "accept"<<"client_socket:"<< client_socket<<std::endl;
        pthread_t tid;
        pthread_create(&tid,NULL,Server::thread_function,&client_socket);
    #elif 0//select方案
        fd_set rset,rfds;
        FD_ZERO(&rfds);
        FD_SET(server_fd,&rfds);
        int maxfd=server_fd;
        
        while (1)
        {
            rset=rfds;
            int nready=select(maxfd+1,&rset,NULL,NULL,NULL);
            if (FD_ISSET(server_fd,&rset))
            {
                
                socklen_t addrlen=sizeof(remote_address);
                client_socket=accept(server_fd,(sockaddr *)&remote_address,&addrlen);
                std::cout << "client_socket"<<client_socket<< std::endl;
                FD_SET(client_socket,&rfds);
                
                //maxfd=client_socket;错误，当序号小的socket重新链接时，maxfd会变为较小值而访问不到大的还在等待的socket
                maxfd=  (maxfd < client_socket) ? client_socket : maxfd;
            }
            for(int i=server_fd+1;i<maxfd+1;i++){
                if (FD_ISSET(i,&rset))
                {
                    char buff[1024]={0};
                    
                    ret=recv(i,(void*)&buff,1024,0);
                    if (ret==0)
                    {
                        std::cout<<"disconnect"<<std::endl;
                        FD_CLR(i,&rfds);
                        close(i);
                        
                    }else if (ret<0)
                    {
                        std::cout<<"error"<<std::endl;
                    }else{
                        std::cout<<buff<<std::endl;
                        ret=send(i,(void *)&buff,ret,0);
                        std::cout<<ret<<std::endl;
                    }
                                        
                }
                
            }
            
        }
    #elif 0  //poll
    struct pollfd fds[1024]={0};
    fds[server_fd].fd=server_fd;
    fds[server_fd].events=POLLIN;
    int maxfd=server_fd;
    while (1)
    {
       int nready=poll(fds,maxfd+1,-1);
       if (fds[server_fd].revents & POLLIN)
       {
            socklen_t addrlen=sizeof(remote_address);
            client_socket=accept(server_fd,(sockaddr *)&remote_address,&addrlen);
            std::cout << "client_socket"<<client_socket<< std::endl;
            fds[client_socket].fd=client_socket;
            fds[client_socket].events=POLLIN;
            maxfd=(maxfd<client_socket) ? client_socket : maxfd;
       }
       for (int i = server_fd+1; i < maxfd+1; i++)
       {
            if (fds[i].revents & POLLIN)
            {
                char buff[1024]={0};
                ret=recv(fds[i].fd,(void *)&buff,1024,0);
                if (ret==0)
                {
                    fds[i].fd=-1;
                    fds[i].events=0;
                    close(i);
                }else if (ret<0)
                {
                    std::cout<<"error"<<std::endl;
                }else
                {
                    std::cout << buff<<std::endl;
                    ret=send(fds[i].fd,(void *)&buff,ret,0);
                }
                
            }
            
       }
    }
    #elif 1
    int epfd=epoll_create(1);
    struct epoll_event ev;
    ev.data.fd=server_fd;
    ev.events=POLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,server_fd,&ev);
    struct epoll_event events[1024]={0};
    while (1)
    {
        int nready=epoll_wait(epfd,events,1024,-1);
        for (int i = 0; i < nready; i++)
        {
            int connfd=events[i].data.fd;
            if (connfd==server_fd)
            {
                socklen_t addrlen=sizeof(remote_address);
                client_socket= accept(server_fd,(sockaddr *)&remote_address,&addrlen);
                std::cout << "client_socket"<<client_socket<< std::endl;
                ev.events=EPOLLIN | EPOLLET;
                ev.data.fd=client_socket;
                epoll_ctl(epfd,EPOLL_CTL_ADD,client_socket,&ev);

            }else if (events[i].events & EPOLLIN)
            {
                char buff[10]={0};
                ret=recv(connfd,(void *)buff,10,0);
                if (ret==0)
                {
                    std::cout <<"disconnect client_socket:"<< connfd<<std::endl;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
                    close(connfd);

                }else if (ret<0)
                {
                    std::cout<<"wrong_recv"<<std::endl;
                }else
                {
                    std::cout<<buff<<std::endl;
                    ret=send(connfd,&buff,ret,0);
                    if (ret<0)
                    {
                        std::cout <<"wrong_send"<<std::endl;
                    }
                    
                }              
            }                 
        }
    }
    #endif
    }
    close(server_fd);
    return ; 
}