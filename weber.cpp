#include "init.h"
#include "http.h"
#include "weber.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include <pthread.h>
#include <sys/epoll.h>


void *func(void *ptr){
    std::cout<<"in thread "<<std::endl;
    int conn = *((int *)ptr);

    char recv_buf[1500];
    char send_buf[1500];
    int ret;
    while(true){
        ret = read(conn,recv_buf,sizeof(recv_buf));
        if(ret <= 0){
            break;
        }
        std::cout<<"read ret is "<<ret<<std::endl;
        Request arequest = Request();
        arequest.parse_head(recv_buf,ret);

        char path[256];
        if(arequest.uri_[1] == 0){
            snprintf(path,256,"%s/index.html",config.root_dir);
        }
        else{
            snprintf(path,256,"%s/%s",config.root_dir,arequest.uri_);
        }
        Response aresponse = Response();
        FILE *fp = fopen(path,"r");
        if(fp == NULL){
            snprintf(path,256,"%s/404.html",config.root_dir);
            std::cout<<"** path is "<<path<<std::endl;
            fp = fopen(path,"r");
            aresponse.set_code(404);
        }
        else{
            std::cout<<"** path is "<<path<<std::endl;
            aresponse.set_code(200);
        }

        /* 读取文件内容 */
        fseek(fp,0,SEEK_END);
        int filesize = ftell(fp);
        char file_buf[512];
        int file_len = 0;
        if(filesize > 512) filesize = 512;
        rewind(fp);
        file_len = fread(file_buf,1,filesize,fp);
        fclose(fp);

        int send_len = 1500;
        aresponse.make_response(send_buf,&send_len,file_buf,file_len);
        write(conn,send_buf,strlen(send_buf));

        // 解keep-alive
        /*
        if not keep-alive:
            break
        */
    }
    close(conn);
    std::cout<<"out thread "<<std::endl;
}

int main(int argc,char **argv){
    arg.init();
    if(arg.parse_args(argc,argv) < 0){
        std::cerr<<"parse args error"<<std::endl;
        return -1;
    }
    if(arg.help) return 0;
    if(config.load_config(arg.config_path) < 0){
        std::cerr<<"load config error"<<std::endl;
        return -1;
    }

    // epoll
    int epollfd = epoll_create1(0);
    struct epoll_event ev;

    /*
     * 开启日志线程，预分配空间
     */

    int listenfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenfd < 0){
        std::cout<<"error when create socket"<<std::endl;
        return -1;
    }
    
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev) < 0){
        // log_error("error when epoll ctl add listenfd")
        close(listenfd);
        close(epollfd);
        return -1;
    }

    struct sockaddr_in ser_addr;
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port=config.port;
    ser_addr.sin_addr.s_addr = config.server;

    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0){
        std::cout<<"err when open reuse addr"<<std::endl;
        return -1;
    }

    if(bind(listenfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))<0){
        std::cerr<<"err when bind"<<std::endl;
        close(listenfd);
        return -1;
    }
    if(listen(listenfd,config.conn_max)<0){
        std::cerr<<"err when listen"<<std::endl;
        close(listenfd);
        return -1;
    }

    struct sockaddr_in peeraddr;
    socklen_t peerlen=sizeof(peeraddr);
    int connfd;
    int nready;
    struct epoll_event events[1024];
    while(true){
        nready = epoll_wait(epollfd,events,1024,-1);
        if(nready == -1){
            // log error
            // clean 
            return -1;
        }
        for(int i = 0;i<nready;i++){
            if(events[i].data.fd == listenfd){
                connfd = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen);
                if(connfd < 0){
                    std::cout<<"err when accept"<<std::endl;
                    continue;
                }
                std::cout<<"recv from "<<inet_ntoa(peeraddr.sin_addr)<<", port is "<<ntohs(peeraddr.sin_port)<<std::endl;
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&ev) < 0){
                    // log_error("error when epoll ctl add connfd")
                    close(connfd);
                    continue;
                }
            }
            else{
                connfd = events[i].data.fd;
                pthread_t pid;
                if(pthread_create(&pid,NULL,func,&connfd) != 0){
                    std::cerr<<"error when create a new thread"<<std::endl;
                    close(connfd);
                    continue;
                }
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                epoll_ctl(epollfd,EPOLL_CTL_DEL,connfd,&ev);
            }
        }
    }
    close(epollfd);
    close(listenfd);
    return 0;
}
