### 网络编程涉及的部分结构体和函数

#### 套接字地址结构 

```c
#include <netinet/in.h>

//通用套接字地址结构(在 bind 、 accept 等套接字函数中使用的结构体)
struct sockaddr{
    sa_family_t sa_family;
    char sa_data[14];
}

//IPv4 套接字地址结构
struct sockaddr_in{
    sa_family_t     sin_family;  // 地址族，对IPv4来说就是 AF_INET
	in_port_t       sin_port;    // 端口
    struct in_addr  sin_addr;    // IPv4 地址
	char sin_zero[8];			 // 0 填充，为了使 sockaddr_in 与 sockaddr 占用空间相同（能实现相互转换）
}

struct in_addr{
    in_addr_t s_addr;    //uint32_t
}
```

​    

#### 字节序转换函数

```c
/* 内存中存储多字节数据时有 `小端字节序`和`大端字节序` 两种方式（可自行查询了解）
 * 网络传输时使用的时大端字节序,在编写 socket 程序时需要进行主机序和网络序的转换。
*/
#include <netinet/in.h>

// 主机序转网络字节序
uint16_t htons(uint16_t v);
uint32_t htonl(uint32_t v);

// 网络字节序转主机序
uint16_t ntohs(uint16_t v);
uint32_t ntohl(uint32_t v);
```



#### 地址转换函数

```c
#include <arpa/inet.h>

//IPv4
int inet_aton(const char* str,struct in_addr *addr); // 将点分十进制格式的 IPv4 字符串转成 32 位的网络字节序的值，成功返回1，否则0
in_addr_t inet_addr(const char *str);  // 功能同上，出错时返回 INADDR_NONE（通常是一个32位全1的值）， 《UNIX网络编程》不建议使用
char *inet_ntoa(struct in_addr inaddr);  // 将32位网络字节序的值转成点分十进制格式，返回值是该字符串的指针。需要注意的是该指针是指向静态内存的，多次调用该函数时修改的是同一块内存
```



#### socket 函数

```c
#include <sys/socket.h>
int socket(int family, int type, int protocol);
```

1. 功能

   创建一个套接字

2. 参数

   - 三个参数分别对应  协议族（常用值有 AF_INET/AF_INET6），套接字类型（常用值有 SOCK_STREAM/SOCK_DGRAM），协议类型（常用值有 IPPROTO_TCP/IPPROTO_UDP）
   - 在编写http服务器时，这三个值我们可以固定取 AF_INET    SOCK_STREAM    IPPROTO_TCP  表示创建处理 IPv4  TCP 协议的流套接字。
   - AF\_* 和 PF\_* 的区别我确实搞不太懂，不过实际上好像也没什么区别

3. 返回

   若成功，返回一个非负值（套接字描述符）；失败返回 -1



#### bind 函数

```c
#include <sys/socket.h>
int bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
```

1. 功能

   bind 函数将一个本地协议地址赋予一个套接字（将地址与套接字绑定起来）

2. 参数

   套接字描述符， 套接字地址结构， 套接字地址结构大小

3. 返回

   成功返回0，失败返回 -1

   

#### listen 函数

```c
#include <sys/socket.h>
int listen(int sockfd,int backlog);
```

1. 功能

   - listen 仅由 TCP 服务器调用，通常在 socket() 和 bind() 之后调用
   - 它将一个主动套接字转换成被动套接字（即将套接字从能发起连接的状态变成能接受连接的状态）

2. 参数

   套接字描述符， 第二个参数可以理解为该套接字的最大连接数量

3. 返回

   成功返回0，失败返回 -1

   

#### accept 函数

```c
#include <sys/socket.h>
int accept(int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);
```

1. 功能

   - 由 TCP 服务端调用
   - 从已完成三次握手的队列中返回一个连接
   - 若队列为空，则阻塞

2. 参数

   - 套接字描述符，套接字地址结构，套接字地址长度
   - cliaddr 和 addrlen 均会被该函数修改。cliaddr 会被设置成客户端的协议地址，addrlen 会置为cliaddr 的实际长度。

3. 返回

   - 成功返回非负值（一个套接字描述符），失败返回 -1
   - accept 返回的是一个新的套接字描述符，表示一个TCP客户端与服务器的连接

4. 其他

   - 要注意区分 accept 函数的第一个参数和返回值两种 套接字描述符的区别，《UNIX网络变成》分别称之为 监听套接字 和 已连接套接字
   - 监听套接字会在TCP服务器的生命周期中一直存在；已连接套接字只在服务器给客户端的一次服务中存在
   - 可以理解成 已连接套接字 才是真正给客户端提供 发送数据等服务的；而监听套接字存在的意义就是给出 已连接套接字

   

#### connect 函数

```c
#include <sys/socket.h>
int connect(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
```

1. 功能

   对于 TCP 套接字，TCP客户端调用 connect 与 TCP 服务器建立连接；调用该函数会激发三次握手

2. 参数

   套接字描述符， 套接字地址结构， 套接字地址结构大小

3. 返回值

   成功返回0，失败返回 -1



#### read 函数

```c
#include <unistd.h>
ssize_t read(int fd,void *buf,size_t count);
```

1. 功能

   从文件描述符 fd 中读取count 字节的数据到 buf 中

2. 参数

   文件描述符， 存放读取内容的 buf，要读取的长度

3. 返回

   成功返回读取的字节数，失败返回 -1



#### write 函数

```c
#include <unistd.h>
ssize_t write(int fd,void *buf,size_t count);
```

1. 功能

   将 buf 缓冲区中 count 字节的内容写入到 fd 中

2. 参数

   文件描述符， 要写入的buf，要写入的长度

3. 返回

   成功返回写入的字节数，失败返回 -1

   

#### close 函数

```c
#include <unistd.h>
int close(int fd);
```

1. 功能

   - 将 fd 的引用计数减 1，引用计数为0时关闭该描述符
   - 单进程网络编程可以认为调用 close 就是关闭该套接字

2. 参数

   一个文件描述符（也可以是套接字描述符）

3. 返回

   成功返回0 ，失败返回 -1