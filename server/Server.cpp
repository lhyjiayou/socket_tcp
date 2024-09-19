#include "Server.h"


Server::Server(/* args */)
{

}

Server::~Server()
{
}
void Server::runServer()//只能监听一个
{

    int server_fd,ret;
    int client_socket;
    struct sockaddr_in address,remote_address;
    char buffer[1024]={0};
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
        socklen_t addrlen = sizeof(remote_address);
        client_socket = accept(server_fd, (struct sockaddr *)&remote_address, &addrlen);

        //client_socket=accept(server_fd,(struct sockaddr *)&remote_address,(socklen_t *)1024); 错误
        /*
        类型不匹配：socklen_t 是一个无符号整数类型，通常用于存储套接字地址的长度。当你将 1024 强制转换为
         socklen_t 类型的指针时，你实际上是创建了一个指向一个整数（在这里是 1024）的指针，而不是一个 socklen_t 
         类型的变量的地址。

        内存访问：指针是用来访问内存位置的，而 (socklen_t *)1024 表达式实际上是一个内存地址，
        它并不代表任何有效的 socklen_t 变量的地址。这个地址可能指向程序的任何部分，甚至是未分配或受保护的内存区域，
        这可能导致未定义行为，包括程序崩溃或数据损坏。

        值的误用：在 accept 函数中，第三个参数应该是一个指向 socklen_t 类型变量的指针，这个变量在函数调用后会包含
        客户端地址的实际长度。如果你传递了一个固定的地址（如 (socklen_t *)1024），那么 accept 函数可能会尝试写入
        这个地址，而不是一个有效的 socklen_t 变量，这会导致错误或未定义行为。
        */


        std::cout<< "accept"<<"client_socket:"<< client_socket<<std::endl;
        while (1)
        {
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
    }
    close(server_fd);
    return ; 

}