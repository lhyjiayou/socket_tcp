#include"Client.h"




Client::Client(/* args */)
{
}

Client::~Client()
{
}

void Client::runClient()
{
    int Client_fd;
    struct sockaddr_in address;
    char buffer[1024]={0};
    char recv_buffer[1024]={0};
    int ret=-1;
    Client_fd=socket(AF_INET,SOCK_STREAM,0);
    if (Client_fd<0)
    {
        std::cout << "wrong" << std::endl;
        return;
    }
    address.sin_family=AF_INET;
    address.sin_port=htons(8080);
    ret=inet_pton(AF_INET,"192.168.19.128",&address.sin_addr);
    ret=connect(Client_fd,(struct sockaddr *)&address,sizeof(address));
    if (ret<0)
    {
        std::cout << "wrong_connect" << std::endl;
        return;
    }
    std::cout<<"Enter message to send to server:"<<std::endl;
    while (1)
    {

        if (!std::cin.getline(buffer, 1024)) {
            std::cout << "Error reading input" << std::endl;
            break; // 如果读取失败，退出循环
        }
        std::string str(buffer);
        if (str=="quit")
        {
            std::cout<< "nonono"<<std::endl;
            break;
        }
        ret=send(Client_fd,(void*)buffer,str.length(),0);
        if (ret==-1)
        {
            std::cout << "wrong_send" << std::endl;
            continue;
        }
        std::cout<<"Message sent to server"<< std::endl;
        ret=recv(Client_fd,(void*)recv_buffer,1024,0);
         if (ret==-1)
        {
            std::cout << "wrong_send" << std::endl;
            continue;

        }
        std::cout << "Message recvice from server"<< recv_buffer << std::endl;
    }

    close(Client_fd);

    
    
}
