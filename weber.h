#ifndef _WEBER_H_
#define _WEBER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <map>
#include <string.h>

ssize_t readn(int fd, void * buff, size_t nbytes){
	size_t nleft=nbytes;
	ssize_t nread = 0;
	char *ptr = static_cast<char *>(buff);

	while(nleft>0){
		if( (nread = read(fd, ptr, nleft)) < 0){
			if(errno == EINTR){
				nread = 0;
			}
			else{
				return -1;
			}
		}
		else if(nread == 0){
			break;
		}
		nleft -= nread;
		ptr += nread;
	}
	return nbytes - nleft;
}

ssize_t writen(int fd, const void * buff, size_t nbytes){
	size_t nleft = nbytes;
	ssize_t nwritten;
	const char *ptr = static_cast<char *>(const_cast<void*>(buff));

	while(nleft > 0){
		if( (nwritten = write(fd,ptr,nleft)) <=0){
			if(nwritten < 0 && errno == EINTR){
				nwritten = 0;
			}
			else{
				return -1;
			}
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return nbytes;
}
class http{
    public:
        char method[16];
        char uri[16];
        char version[16];
        char host[16];
        std::map<std::string,std::string> header_;
        /*
        std::string Host;
        std::string Accept;
        std::string Accept-Encoding;
        std::string Accept-Language;
        */
        http(){
            memset(method,"GET",16);
            memset(uri,"/",16);
            memset(version,"HTTP/1.1",16);
        }
};




#endif
