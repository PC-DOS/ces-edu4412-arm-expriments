/* client.c */
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
    strcpy(buf,"I miss you already!\n");  
	/* 创建socket */
    if((ssock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){  
        perror("socket error:\n");  
        exit(1);  
    }  
    memset(&server_addr,0,sizeof(server_addr));  
	/* 设置sockaddr_in 结构体中相关参数 */
    server_addr.sin_family  =AF_INET;  
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");  
    server_addr.sin_port=htons(7758); 
	/* 发送数据给服务器端 */ 
    sendto(ssock,(void *)buf,MAXBUF,0,(struct sockaddr*)&server_addr,sizeof(server_addr));  
    printf("Send message:%s\n",buf);  
    clen=sizeof(client_addr);  
	/* 接收服务器端发送过来的数据 */
    recvfrom(ssock,(void *)buf,MAXBUF,0,(struct sockaddr*)&server_addr,&clen);  
    printf("Receive message:%s\n",buf);  
    close(ssock);  
    return 0;  
} 
