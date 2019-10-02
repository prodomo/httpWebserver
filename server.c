#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http.h"

#define MAXBUFFSIZE 1024

void send_response(int con_id, int status, char *value)
{
    char buff[MAXBUFFSIZE];
    sprintf(buff, "HTTP/1.1 %d %s\r\n Content-Type: text/html\r\n\r\n %s", status, status_code_strings.find(status), 
    "<html><head><meta charset=\"utf-8\"><title>http webserver-test</title></head><body>Hello !!</body></html>");
}

int main(int argc , char *argv[])
{
    int socketR = 0, clientR;
    int backlogNum =5;
    char inputBuffer[256]={};
    char message[]={};
    struct sockaddr_in server_info, client_info;
    int addrlen = sizeof(client_info);

    //AF_INET=IPv4 AF_INET6=IPv6, SOCK_STREAM=connection SOCK_DGRAM=connectionless
    socketR = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socketR == -1)
    {
        printf("[ERROR] Fail to create a socket! \n");
    }

    bzero(&server_info, sizeof(server_info)); //init set info all bit to zero
    server_info.sin_family  = AF_INET;
    server_info.sin_port = htons(80);
    server_info.sin_addr.s_addr = INADDR_ANY; //localIP decided by kernal

    bind(socketR, (struct sockaddr*)&server_info, sizeof(server_info));
    listen(socketR, backlogNum);

    while(1)
    {
        clientR = accept(socketR, (struct sockaddr*)&client_info, &addrlen);
        recv(clientR, inputBuffer,sizeof(inputBuffer),0);
        printf("Get:%s\n",inputBuffer);
        send(clientR, message, sizeof(message), 0);
        printf("send:%s\n",message);
        close(clientR);
    }

    return 0;
}
