#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define MAXBUFFSIZE 1024

char response_header_200[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
const char response_header_404[] = "HTTP/1.1 404 Not Found\r\n";
char htmldata[]="<html><head><meta charset=\\utf-8\\><title>http webserver-test</title></head><body>Hello!!</body></html>\r\n\r\n";

struct http_response{
    char *status;
    char *content_type;
    char *content_type_v;
    // char *content_length;
    // char *content_length_v;
};

void send_response(int con_id, int status)
{
    int size=0;
    struct http_response respon;
    if(status==200){
        respon.status="HTTP/1.1 200 OK\r\n";
        respon.content_type="Content-Type: ";
        respon.content_type_v="text/plain\r\n\r\n";

        size = strlen(respon.status)+strlen(respon.content_type)+strlen(respon.content_type_v)+strlen(htmldata);
        // printf("%d %d %d %d %d\n",size ,strlen(respon.status),strlen(respon.content_type),strlen(respon.content_type_v),strlen(htmldata));
        char* buff = malloc(sizeof(char)*size);
        sprintf(buff, "%s%s%s%s", respon.status, respon.content_type, respon.content_type_v, htmldata);
        send(con_id, buff, size, 0);
        close(con_id);
        free(buff);
    }
    else if(status==404)
    {
        respon.status="HTTP/1.1 404 not found\r\n\r\n\r\n";
        respon.content_type="";
        respon.content_type_v="";

        size = strlen(respon.status)+strlen(respon.content_type)+strlen(respon.content_type_v);
        // printf("%d %d %d %d %d\n",size ,strlen(respon.status),strlen(respon.content_type),strlen(respon.content_type_v),strlen(htmldata));
        char* buff = malloc(sizeof(char)*size);
        sprintf(buff, "%s%s%s", respon.status, respon.content_type, respon.content_type_v);
        send(con_id, buff, size, 0);
        close(con_id);
        free(buff);
    }
}

void get_filename(char* rcv, char* name)
{
    char *stopword = "\n";
    char *pch;
    int tempsize=0;
    char* line;

    printf("get %s\n", rcv);
    pch = strtok(rcv, stopword);
    tempsize = strlen(pch);
    line = malloc(sizeof(char)*tempsize);
    strncpy(line, pch, sizeof(char)*tempsize);
    printf("line %s\n", line);
    
    // sscanf(line, "GET %s HTTP/1.1", name);
    name = strtok(line, " ");
    name = strtok(NULL, " ");
    printf("get filename: %s\n", name);
}



int main(int argc , char *argv[])
{
    int socketR = 0, clientR;
    int backlogNum =5;
    char inputBuffer[256]={};
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
        char* filename;
        printf("Get:%s\n",inputBuffer);
        get_filename(inputBuffer, filename);
        // send(clientR, message, sizeof(message), 0);
        send_response(clientR, 200);
    }

    return 0;
}
