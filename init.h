#ifndef _INIT_H_
#define _INIT_H_

#include <unistd.h>
#include <string>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <stdio.h>

class Arg{
public:
    std::string config_path;
    bool help;
    Arg(){};
    void usage(){
        std::cout<<"Usage:"<<std::endl;
        std::cout<<"    ./exec -c config_path"<<std::endl;
        std::cout<<std::endl;
    }
    void init(){
        config_path = "./weber.conf";
        help = false;
    }
    int parse_args(int argc,char **argv){
        int opt;
        const char *optstr = "c:h";

        while((opt = getopt(argc,argv,optstr)) != -1){
            switch(opt){
                case 'c':
                    config_path = optarg;
                    break;
                case 'h':
                    usage();
                    return 1;
                default:
                    std::cout<<"unknown opt "<<opt<<std::endl;
                    return -1;
            }

        }
        return 0;
    }
};

class Config{
public:
    char root_dir[32];
    uint32_t server;
    short port;
    int conn_max;

    Config(){}

    int load_config(const char *path){
        //FILE *fp = fopen(path,"r");
        //fclose(fp);

        //root_dir = "./site";
        snprintf(root_dir,32,"./site");
        //server = 0;
        std::cout<<"aaa"<<std::endl;
        if(inet_aton("0.0.0.0",(struct in_addr*)(&server)) < 0) throw "err when inet_aton";
        std::cout<<"bbb"<<std::endl;
        port = htons(12138);
        conn_max = (1 << 31) - 1;
        return 0;
    }
};

#endif
