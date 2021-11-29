#ifndef _WEBER_H_
#define _WEBER_H_

#include <unistd.h>

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

#endif
