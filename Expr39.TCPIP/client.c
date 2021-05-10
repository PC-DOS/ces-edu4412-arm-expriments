/* client.c */  
#include<stdio.h>  
#include<unistd.h>  
#include<stdlib.h>  
#include<sys/socket.h>  
#include<sys/stat.h>  
#include<arpa/inet.h>  
#include<string.h>
#define MAXBUF 256  
int main(int argc, char *argv[])  
{  
    int ssock;  
    int clen, n;  
    struct sockaddr_in server_addr;  
    char buf[MAXBUF];
    /*  创建套接字  */  
    if((ssock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0){  
        perror("socket error:");  
        exit(1);  
    }  
    clen = sizeof(server_addr);  
    memset(&server_addr,0,sizeof(server_addr)); 
    /* 设置sockaddr_in结构体中相关参数 */ 
    server_addr.sin_family     =AF_INET;  
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");  
    server_addr.sin_port       =htons(8888);  
	/* 建立连接 */
    if(connect(ssock,(struct sockaddr *)&server_addr,clen)<0){  
        perror("connect error:");  
        exit(1);  
    }  
    memset(buf,0,MAXBUF);  
	/* 发送数据 */
    while(1){
    	fgets(buf, MAXBUF, stdin);
    	printf("buf:%s",buf);
    	if(-1 == (n = send(ssock, buf, strlen(buf), 0)))
    	{
        	perror("sendto");
        	exit(-1);
    	}
    	if(strncmp(buf, "quit", 4) == 0)
            break;
	    printf("n = %d\n",n);
    }

    close(ssock);  
    printf("\nread: %s\n",buf);  
    return 0;  
} 
