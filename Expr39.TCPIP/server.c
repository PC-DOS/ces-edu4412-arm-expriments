/* server.c */
#include<stdio.h>  
#include<unistd.h>  
#include<stdlib.h>  
#include<string.h>  
#include<sys/socket.h>  
#include<sys/stat.h>  
#include<arpa/inet.h>  
#define MAXBUF 256  
int main(int argc, char *argv[])  
{  
    int clen, n;  
    int ssock,csock;  
    struct sockaddr_in client_addr,server_addr;  
    char buf[MAXBUF];
    /* 建立套接字连接 */	  
    if((ssock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0){  
        perror("socket error:");  
        exit(1);  
    }  
    clen = sizeof(client_addr);  
    memset(&server_addr,0,sizeof(server_addr));
    /* 设置sockaddr_in结构体中相关参数 */  
    server_addr.sin_family     =AF_INET;  
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(8888);
    /* 绑定函数bind（） */  
    if(bind(ssock,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){  
        perror("bind error");  
        exit(1);  
    }
    /* 调用listen（）函数，设置监听模式 */  
    if(listen(ssock,8)<0){  
        perror("listen error:");  
        exit(1);  
    }  
    /* 调用accept（）函数，等待客户端的连接 */
    if(-1 == (csock = accept(ssock, (struct sockaddr *)&client_addr, &clen))) {
        perror("accept");
        exit(-1);
    }
    /* 调用recv（）函数接收客户端发送的数据 */
	while(1) {
	    n = recv(csock, buf, MAXBUF, 0);
    	buf[n] = 0;
    	printf("n = %d---%s\n",n,buf);
    	if(0 == strncmp(buf,"quit",4))
			break;
	    send(csock, buf, strlen(buf), 0);
    }

    return 0;  
} 
