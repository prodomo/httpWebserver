#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>

#define MAXBUFFSIZE 1024
#define M_GET 1

// char response_header_200[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
// const char response_header_404[] = "HTTP/1.1 404 Not Found\r\n";
// char htmldata[]="<html><head><meta charset=\\utf-8\\><title>http webserver-test</title></head><body>Hello!!</body></html>\r\n\r\n";
char cwd[PATH_MAX];

struct http_response{
    char *status;
    char *content_type;
    char *content_type_v;
    char *content_length;
    char *content_length_v;
    char *end;
};
struct http_request{
    int method;
    char* url;
    char* payload;
};

void send_response(int con_id, int status, int packetsize)
{
    int size=0;
    struct http_response respon;
    if(status == 200){
        respon.status="HTTP/1.1 200 OK\r\n";
        respon.content_type="Content-Type: ";
        respon.content_type_v="text/html\r\n";
        respon.content_length="Content-Length: ";
        respon.end="\r\n\r\n";
        sprintf(respon.content_length_v, "%d", packetsize);

        // size = strlen(respon.status)+strlen(respon.content_type)+strlen(respon.content_type_v)+strlen(htmldata);
        size = strlen(respon.status)+strlen(respon.content_type)+strlen(respon.content_type_v)+strlen(respon.content_length)+strlen(respon.content_length_v)+strlen(respon.end);
        // printf("%d %d %d %d %d\n",size ,strlen(respon.status),strlen(respon.content_type),strlen(respon.content_type_v),strlen(htmldata));
        char* buff = malloc(sizeof(char)*size);
        sprintf(buff, "%s%s%s%s%s%s", respon.status, respon.content_type, respon.content_type_v, respon.content_length, respon.content_length_v, respon.end);
        send(con_id, buff, size, 0);
        free(buff);
    }
    else if(status == 404)
    {
        size=0;
        respon.status="HTTP/1.1 404 Not found\r\n";
        respon.content_type="";
        respon.content_type_v="";
        respon.content_length="Content-Length: ";
        respon.content_length_v="0";
        respon.end="\r\n\r\n";
        
        // printf("each strlen %ld %ld %ld %ld\n", strlen(respon.status), strlen(respon.content_length), strlen(respon.content_length_v),strlen(respon.end));
        size = strlen(respon.status) + strlen(respon.content_length) + strlen(respon.content_length_v) + strlen(respon.end);
        // printf("size of total = %d\n\n", size);
        char* buff = malloc(sizeof(char)*size);
        sprintf(buff, "%s%s%s%s", respon.status, respon.content_length, respon.content_length_v, respon.end);
        send(con_id, buff, size, 0);
        free(buff);
    }
    else{

    }
}

void rcv_handler(char* rcv, int con_id)
{
    char *stopword = "\r\n";
    char *pch;
    int tempsize=0;
    char* line;
    char *name;
    int method;
    size_t ret;

    printf("get %s\n", rcv);
    pch = strtok(rcv, stopword);
    tempsize = strlen(pch);
    line = malloc(sizeof(char)*tempsize); 
    strncpy(line, pch, sizeof(char)*tempsize); //keep the first line of request
    printf("line %s\n", line);
    
    name = strtok(line, " ");
    if(strncmp(name, "GET", 3)==0)
    {
        name = strtok(NULL, " ");
        printf("get filename: %s\n", name);
        method = M_GET;

        FILE *fp;
        char buffer[1024];
        long filesize = 0;
        char * wholename = malloc(strlen(cwd)+strlen(name));
        sprintf(wholename, "%s%s", cwd, name);
        printf("path: %s\n", wholename);
        if((fp=fopen(wholename, "r")) != NULL)
        {
            printf("open file!! ");
            /*using file end and file head to get file size*/
            fseek(fp, 0, SEEK_END);
            filesize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            printf("filesize: %ld\n", filesize);

            send_response(con_id, 200, filesize);
            if(MAXBUFFSIZE > filesize)
            {
                ret = fread(buffer, filesize, 1, fp);
                send(con_id, buffer, filesize, 0);
            }
            else
            {
                int temp = filesize;
                ret = fread(buffer, MAXBUFFSIZE, 1, fp);
                send(con_id, buffer, MAXBUFFSIZE, 0);
                while(ret >0)
                {
                    temp = temp - MAXBUFFSIZE;
                    if(temp > MAXBUFFSIZE)
                    {
                        ret = fread(buffer, MAXBUFFSIZE, 1, fp);
                        send(con_id, buffer, MAXBUFFSIZE, 0);
                    }
                    else{
                        ret = fread(buffer, temp, 1, fp);
                        send(con_id, buffer, temp, 0);
                    }
                }
            }
            fclose(fp);
            free(wholename);
        }
        else
        {
            filesize=0;
            printf("cann't open file\n");
            send_response(con_id, 404, filesize);
            
            free(wholename);
        }
    }
    free(line);  
}

int main(int argc , char *argv[])
{
    int socketR = 0, clientR;
    int backlogNum =5;
    char inputBuffer[256]={};
    struct sockaddr_in server_info, client_info;
    int addrlen = sizeof(client_info);

    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working dir: %s\n", cwd);
    }else{
        printf("getcwd() error");
    }

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
        rcv_handler(inputBuffer, clientR);
        // send(clientR, message, sizeof(message), 0);
        close(clientR);
    }

    return 0;
}
