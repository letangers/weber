#include "weber.h"
#include <netdb.h>
#include <iostream>








int main(){
    int sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock < 0){
        std::cout<<"error wehn create socker"<<std::endl;
        return -1;
    }

    struct sockaddr_in ser_addr;
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port=htons(12138);
    //ser_addr.sin_port=htons(80);
    std::string host = "baidu.com";
    //struct hostent *p_host = gethostbyname(host.c_str());
    //memcpy(&(ser_addr.sin_addr),p_host->h_addr_list[0],sizeof(ser_addr.sin_addr));
    ser_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if(connect(sock,(struct sockaddr*)&ser_addr,sizeof(ser_addr))<0){
        std::cout<<"error when connect to "<<host<<std::endl;
        return -1;
    }

    char send_buf[65535] = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en,zh-CN;q=0.9,zh;q=0.8,en-US;q=0.7\r\nUser-Agent: Mozilla/5.0 (X11; CrOS x86_64 14324.13.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.20 Safari/537.36\r\n\r\n";
    //int nbyte = writen(sock,send_buf,strlen(send_buf));
    int nbyte = write(sock,send_buf,strlen(send_buf));
    std::cout<<"send "<<nbyte<<"  "<<strlen(send_buf)<<std::endl;

    char recv_buf[65535];
    //nbyte = readn(sock,recv_buf,65535);
    nbyte = read(sock,recv_buf,65535);
    std::cout<<"recv "<<nbyte<<std::endl;
    std::cout<<recv_buf<<std::endl;

    close(sock);
    return 0;
}
