#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>
#include "lib/log.h"

#define MAXBUFFSIZE 1024
#define M_GET 1


struct http_response{
    char *status;
    // char *connection;
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

char cwd[PATH_MAX];
int total_connection_counter;
int total_request_counter;
int total_response_counter;
int current_connction_counter;

void send_response(int con_id, int status, int packetsize)
{
    total_response_counter = total_response_counter + 1;
    LOG_INFO("send response(%d), total response:%d \n",status , total_request_counter);
    int size=0;
    struct http_response respon;
    if(status == 200){
        respon.status="HTTP/1.0 200 OK\r\n"; //len:17
        // respon.connection="Connection: close\r\n"; //len:19
        respon.content_type="Content-Type: "; //len:14
        respon.content_type_v="text/html\r\n";
        respon.content_length="Content-Length: "; //len:16
        respon.end="\r\n\r\n";
        sprintf(respon.content_length_v, "%d", packetsize);

        size = 17+14+strlen(respon.content_type_v)+16+strlen(respon.content_length_v)+4;
        // printf("%d %d %d\n",strlen(respon.status),strlen(respon.content_type),strlen(respon.content_length));
        char* buff = malloc(sizeof(char)*size);
        sprintf(buff, "%s%s%s%s%s%s", respon.status, respon.content_type, respon.content_type_v, respon.content_length, respon.content_length_v, respon.end);
        send(con_id, buff, size, 0);
        free(buff);
    }
    else if(status == 404)
    {
        size=0;
        respon.status="HTTP/1.0 404 Not found\r\n";
        // respon.connection="Connection: close\r\n";
        respon.content_type="";
        respon.content_type_v="";
        respon.content_length="Content-Length: ";
        respon.content_length_v="0";
        respon.end="\r\n\r\n";
        
        // printf("each strlen %ld %ld %ld %ld\n", strlen(respon.status), strlen(respon.content_length), strlen(respon.content_length_v),strlen(respon.end));
        size = 45;
        // size = strlen(respon.status) + strlen(respon.content_length) + strlen(respon.content_length_v) + strlen(respon.end);
        // LOG_DBG("size of total = %d\n\n", size);
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

    LOG_DBG("rcv_handler get %s\n", rcv);
    pch = strtok(rcv, stopword);
    LOG_DBG("pch: %s\n", pch);
    tempsize = strlen(pch);
    line = calloc(tempsize, sizeof(char));
    strncpy(line, pch, tempsize); //keep the first line of request
    LOG_DBG("copy from pch: %s\n", line);
    
    name = strtok(line, " ");
    if(strncmp(name, "GET", 3)==0)
    {
        name = strtok(NULL, " ");
        LOG_DBG("get filename: %s\n", name);
        method = M_GET;

        FILE *fp;
        char buffer[1024];
        long filesize = 0;
        char * wholename = malloc(strlen(cwd)+strlen(name));
        sprintf(wholename, "%s%s", cwd, name);
        LOG_DBG("path: %s\n", wholename);
        if((fp=fopen(wholename, "r")) != NULL)
        {
            LOG_DBG("open file!! \n");
            /*using file end and file head to get file size*/
            fseek(fp, 0, SEEK_END);
            filesize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            LOG_DBG("filesize: %ld\n", filesize);

            send_response(con_id, 200, filesize);
            if(MAXBUFFSIZE > filesize)
            {
                ret = fread(buffer, filesize, 1, fp);
                int check = send(con_id, buffer, filesize, 0);
                LOG_DBG("check send value %d\n", check);
                LOG_DBG("send data packet! packet filesize size:%d \n", filesize);
            }
            else
            {
                int temp = filesize;
                ret = fread(buffer, MAXBUFFSIZE, 1, fp);
                send(con_id, buffer, MAXBUFFSIZE, 0);
                temp = temp - MAXBUFFSIZE;
                while(temp>0 && ret>0)
                {
                    if(temp > MAXBUFFSIZE)
                    {
                        ret = fread(buffer, MAXBUFFSIZE, 1, fp);
                        send(con_id, buffer, MAXBUFFSIZE, 0);
                        LOG_DBG("send data packet! temp size %d packet size:%d \n",temp ,MAXBUFFSIZE);
                    }
                    else if(temp <= MAXBUFFSIZE){
                        ret = fread(buffer, temp, 1, fp);
                        send(con_id, buffer, temp, 0);
                        LOG_DBG("send data packet! packet temp size:%d \n", temp);
                    }
                    else
                    {
                        //pass
                    }
                    temp = temp - MAXBUFFSIZE;
                }
            }
            fclose(fp);
            free(wholename);
        }
        else
        {
            filesize=0;
            LOG_INFO("cann't find file or cann't open file\n");
            send_response(con_id, 404, filesize);

            free(wholename);
        }
    }
    free(line);  
}

int main(int argc , char *argv[])
{
    int socketR = 0, clientR, on=1;
    int backlogNum =5;
    char inputBuffer[256]={};
    struct sockaddr_in server_info, client_info;
    int addrlen = sizeof(client_info);

    total_connection_counter = 0;
    total_request_counter = 0;
    total_response_counter = 0;
    current_connction_counter =0;

    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        // printf("Current working dir: %s\n", cwd);
        LOG_DBG("Current working dir: %s\n", cwd);
    }else{
        LOG_ERR("getcwd() error");
    }

    //AF_INET=IPv4 AF_INET6=IPv6, SOCK_STREAM=connection SOCK_DGRAM=connectionless
    socketR = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socketR == -1)
    {
        // printf("[ERROR] Fail to create a socket! \n");
        LOG_ERR("fail to create a socket! \n");
        exit(EXIT_FAILURE);
    }
    if(setsockopt( socketR, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0)
    {
        LOG_ERR("set socket address resuse error! \n");
        exit(EXIT_FAILURE);
    }

    bzero(&server_info, sizeof(server_info)); //init set info all bit to zero
    server_info.sin_family  = AF_INET;
    server_info.sin_port = htons(80);
    server_info.sin_addr.s_addr = INADDR_ANY; //localIP decided by kernal

    if(bind(socketR, (struct sockaddr*)&server_info, sizeof(server_info))<0)
    {
        LOG_ERR("bind error! \n");
        exit(EXIT_FAILURE);
    }
    if(listen(socketR, backlogNum)<0)
    {
        LOG_ERR("listen error! \n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        clientR = accept(socketR, (struct sockaddr*)&client_info, &addrlen);
        total_connection_counter += 1;
        current_connction_counter += 1;
        LOG_INFO("accept a new connection! total: %d, current: %d\n", total_connection_counter, current_connction_counter);
        
        recv(clientR, inputBuffer,sizeof(inputBuffer),0);
        total_request_counter += 1;
        LOG_INFO("get new request, total request %d\n", total_request_counter);
        // printf("Get:%s\n",inputBuffer);
        LOG_DBG("Get request:%s\n",inputBuffer);
        rcv_handler(inputBuffer, clientR);
        bzero(inputBuffer, sizeof(inputBuffer));
        // send(clientR, message, sizeof(message), 0);
        close(clientR);
        current_connction_counter = current_connction_counter - 1;
        LOG_INFO("close connection, current: %d, total: %d\n", 
        current_connction_counter, total_connection_counter);
    }

    return 0;
}
