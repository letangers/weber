#ifndef _INIT_H_
#define _INIT_H_

#include <unistd.h>
#include <string>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>
#include <unordered_map>


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

class ConfigNode{
    private:
        std::unordered_map<std::string,std::string> fields; 
        
    public:
        ConfigNode(){}
        int set_field(std::string key,std::string value){
            fields[key] = value;
            return 0;
        }

        void show_fields(){
            for(auto &one : fields){
                std::cout<<"\t"<<one.first<<"\t"<<one.second<<std::endl;
            }
        }
};

class Config{
    private:
        std::string strip(std::string &str){
            int begin = str.find_first_not_of(" \t\r\n");
            int end = str.find_last_not_of(" \t\r\n");
            begin = (begin < 0) ? 0 : begin;
            end = (end < 0) ? str.length() : end;
            return str.substr(begin,end+1-begin);
        }
        std::string get_real_line(std::string str){
            //int pos = str.find_first_of("#");
            int pos = str.find("#");
            if(pos != -1){
                str = str.substr(0,pos);
            }
            //pos = str.find_first_of("//");
            pos = str.find("//");
            if(pos != -1){
                str = str.substr(0,pos);
            }
            int begin = str.find_first_not_of(" \t\r\n");
            int end = str.find_last_not_of(" \t\r\n");
            begin = (begin < 0) ? 0 : begin;
            end = (end < 0) ? str.length() : end;
            return str.substr(begin,end+1-begin);
        }

        std::unordered_map<std::string,ConfigNode> conf;
    public:

        char root_dir[32];
        uint32_t server;
        short port;
        int conn_max;

        Config(){}

        int load_config(const std::string path){
            if(path.empty()){
                std::cerr<<"config path is empty"<<std::endl;
                return -1;
            }
            std::ifstream fin(path);
            if(!fin.is_open()){
                std::cerr<<"open file "<<path<<" failed"<<std::endl;
                return -1;
            }

            std::string line,top,key,value;
            int pos;
            bool comment = false;
            while(std::getline(fin,line)){
                line = get_real_line(line);
                pos = line.find("/*");
                if (pos != -1 && pos != 0) return -1;
                if(pos == 0){
                    if (line.find("*/") != -1) continue;
                    comment = true;
                    continue;
                }
                if(comment){
                    if(line.find("*/") == -1) continue;
                    comment = false;
                    continue;
                }

                if(line.length() == 0) continue;
                if((!comment) && line.length() == 1) return -1;


                if(line[0] == '['){
                    if(( pos =line.find(']') ) == -1) return -1;
                    if(pos == 1) continue;
                    top = line.substr(1,pos-1);
                    conf[top] = ConfigNode();
                    continue;
                }

                pos = line.find("=");
                key = line.substr(0,pos);
                value = line.substr(pos+1,line.length() - pos - 1);
                key = strip(key);
                value = strip(value);
                if(top.length() == 0) return -1;
                conf[top].set_field(key,value);
            }
            fin.close();

            snprintf(root_dir,32,"./site");
            if(inet_aton("0.0.0.0",(struct in_addr*)(&server)) < 0) throw "err when inet_aton";
            port = htons(12138);
            conn_max = (1 << 31) - 1;

            return 0;
        }

        void show_config(){
            for(auto &one : conf){
                std::cout<<one.first<<std::endl;
                one.second.show_fields();
            }
        }

        // 获取配置,若无给出默认值
        std::string & get_config(const std::string &top,const std::string &key){


        }
};

Arg arg = Arg();
Config config = Config();
#endif
