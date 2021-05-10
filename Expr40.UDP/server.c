/* server.c */
#include<stdio.h>  
#include<unistd.h>  
#include<stdlib.h>  
#include<string.h>  
#include<sys/socket.h>  
#include<sys/stat.h>  
#include<arpa/inet.h>  
#define MAXBUF 256  
int main()  
{  
    int ssock;  
    int clen;  
    struct sockaddr_in client_addr,server_addr;  
    char buf[MAXBUF];
	/* 建立socket连接 */  
    if((ssock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){  
        perror("socket error:");  
        exit(1);  
    }  
    memset(&server_addr,0,sizeof(server_addr));  
	/* 设置sockaddr_in结构体中相关参数 */
    server_addr.sin_family=AF_INET;  
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);  
    server_addr.sin_port=htons(7758);
	/* 绑定函数bind（） */  
    if(bind(ssock,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){  
        perror("bind error:");  
        exit(1);  
    }  
    clen=sizeof(client_addr);  
	/* 调用recvfrom（）函数，等待接受客户端的数据 */
    recvfrom(ssock,(void *)buf,MAXBUF,0,(struct sockaddr*)&client_addr,&clen);  
    strcpy(buf,"I miss you");  
	/* 调用sendto（）函数，将数据发送给客户端 */
    sendto(ssock,(void *)buf,MAXBUF,0,(struct sockaddr*)&client_addr ,sizeof(client_addr));  
    close(ssock);  
    return 0;  
}
