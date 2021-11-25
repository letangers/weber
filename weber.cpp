#include "weber.h"
#include <iostream>








int main(){
    int listenfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenfd < 0){
        std::cout<<"error wehn create socket"<<std::endl;
        return -1;
    }
    struct sockaddr_in ser_addr;
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port=htons(12138);
    ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0){
        std::cout<<"err when open reuse addr"<<std::endl;
        return -1;
    }

    if(bind(listenfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))<0){
        std::cout<<"err when bind"<<std::endl;
        close(listenfd);
        return -1;
    }
    if(listen(listenfd,SOMAXCONN)<0){
        std::cout<<"err when listen"<<std::endl;
        close(listenfd);
        return -1;
    }

    struct sockaddr_in peeraddr;
    socklen_t peerlen=sizeof(peeraddr);
    while(true){
        char recv_buf[65535];
        char send_buf[65535];
        char html_buf[1024] = "<html><body><h1>我的第一个网页</h1></body></html>\r\n";
        sprintf(send_buf,"HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\nContent-Length: %d\r\n\r\n%s",strlen(html_buf),html_buf);
        int conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen);
        if(conn < 0){
            std::cout<<"err when accept"<<std::endl;
            close(listenfd);
            return -1;
        }
        read(conn,recv_buf,65535);
        std::cout<<recv_buf<<std::endl;
        write(conn,send_buf,strlen(send_buf));
        close(conn);
        break;
    }
    close(listenfd);
    return 0;
}
