#ifndef _HTTP_H_
#define _HTTP_H_

#include <string>
#include <string.h>
#include <map>
#include <stdio.h>


class Request{
    public:
        char method_[8];
        char uri_[256];
        char version_[16];
        char host_[64];
        std::map<std::string,std::string> header_;
        /*
        std::string Host;
        std::string Accept;
        std::string Accept-Encoding;
        std::string Accept-Language;
        */
        request(){
            memset(method_,0,sizeof(method_));
            memset(uri_,0,sizeof(uri_));
            memset(version_,0,sizeof(version_));
        }
        int readline(char *recv_buf){
        
        }

        // 读取第一行内容
        int parse_head(char *buf,int len){
            char b=0,c=0;
            int i = 0;
            int w = 0;
            for(;i < len;i++){
                if(buf[i] == ' ') break;
                if(buf[i] == '\r' || buf[i] == '\n') return -1;
                if(w == sizeof(method_)) return -2;
                method_[w++] = buf[i];
            }
            w = 0;
            for(i++;i<len;i++){
                if(buf[i] == ' ') break;
                if(buf[i] == '\r' || buf[i] == '\n') return -1;
                if(w == sizeof(uri_)) return -2;
                uri_[w++] = buf[i];
            }
            w = 0;
            for(i++;i<len;i++){
                if(buf[i] == ' ') return -1;
                if(buf[i] == '\r'){
                    if(i == (len-1)) return -1;
                    if(buf[i+1] != '\n') return -1;
                    break;
                }
                if(w == sizeof(version_)) return -2;
                version_[w++] == buf[i];
            }
            if((method_[0] | uri_[0] | version_[0]) != 0) return -1;
            return 0;
        }
        // 允许请求体出现 \r|\n,不允许头部出现
        int read_header(){
        
        }
};

class Response{
    public:
        char version_[16];
        char status_[16];
        unsigned int code_;
        
        response(){}
        int set_code(int code){
            if (code == 200){
                snprintf(version_,sizeof(version_),"HTTP/1.1");
                code_ = code;
                snprintf(status_,sizeof(status_),"OK");
                return 0;
            }
            if (code == 404){
                snprintf(version_,sizeof(version_),"HTTP/1.1");
                code_ = code;
                snprintf(status_,sizeof(status_),"Not Found");
                return 0;
            }
            code_ = 500;
            snprintf(version_,sizeof(version_),"HTTP/1.1");
            snprintf(status_,sizeof(status_),"Server Error");
            return 0;
        }
        int make_response(char *send_buf,int *buf_len,char *content,int content_len){
            int max_len = *buf_len;
            *buf_len = snprintf(send_buf,*buf_len,"%s %d %s\r\nContent-Length: %d\r\n\r\n%s",version_,code_,status_,content_len,content);
            if(*buf_len >= max_len) return -1;
            return 0;
        }
};


#endif
