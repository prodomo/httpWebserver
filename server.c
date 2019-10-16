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
#define MINBUFFSIZE 10
#define URLSIZE 100

enum {
    not_support=0,
    html,
    json
};

struct http_response{
    char *status;
    char *connection;
    char *content_type;
    char *content_type_v;
    char *content_length;
    char *content_length_v;
    char *end;
};
struct http_request{
    char* startline;
    size_t startline_len;
    char* header;
    size_t header_len;
    char* payload;
};

struct http_post{
    size_t content_len;
    int content_type;
    int multipart;
    char* boundary;
    char* fname;
};

typedef struct header_s {
    char *name;
    char *value;
    struct header_s *next;
} header_t;


char cwd[PATH_MAX];
int total_connection_counter;
int total_request_counter;
int total_response_counter;
int current_connction_counter;

void split(char **arr, char *str, const char *del)
{
    char *s = strtok(str, del);
    while(s != NULL) {
        *arr++ = s;
        s = strtok(NULL, del);
    }
}

void send_response(int con_id, int status, int packetsize)
{
    total_response_counter = total_response_counter + 1;
    LOG_INFO("send response(%d), total response:%d \n",status , total_request_counter);
    int size = 0;
    struct http_response respon;
    if(status == 200){
        char tempbuff[MINBUFFSIZE];
        respon.status = "HTTP/1.1 200 OK\r\n"; 
        respon.connection = "Connection: close\r\n"; 
        respon.content_type = "Content-Type: "; 
        respon.content_type_v = "text/html\r\n";
        respon.content_length = "Content-Length: "; 
        respon.end = "\r\n\r\n";
        snprintf(tempbuff, (MINBUFFSIZE-1),"%d", packetsize);
        respon.content_length_v = tempbuff;

        LOG_DBG("respon.content_length_v: %s recv packetsize: %d\n", respon.content_length_v, packetsize);
        char* buff = malloc(sizeof(char)*MAXBUFFSIZE);
        int realsize = snprintf(buff, MAXBUFFSIZE-1, "%s%s%s%s%s%s%s", respon.status, respon.content_type, respon.content_type_v, respon.connection, respon.content_length, respon.content_length_v, respon.end);
        send(con_id, buff, realsize, MSG_MORE);
        free(buff);
    }
    else if(status == 404){
        size=0;
        respon.status = "HTTP/1.1 404 Not found\r\n";
        respon.connection = "Connection: close\r\n";
        respon.content_type = "";
        respon.content_type_v = "";
        respon.content_length = "Content-Length: ";
        respon.content_length_v = "0";
        respon.end = "\r\n\r\n";
        
        char* buff = malloc(sizeof(char)*MAXBUFFSIZE);
        int realsize = snprintf(buff, (MAXBUFFSIZE-1), "%s%s%s%s%s", respon.status, respon.connection, respon.content_length, respon.content_length_v, respon.end);
        send(con_id, buff, realsize, 0);
        free(buff);
    }
    else if(status == 415){
        size=0;
        respon.status = "HTTP/1.1 415 Unsupported Media Type\r\n";
        respon.connection = "Connection: close\r\n";
        respon.content_type = "";
        respon.content_type_v = "";
        respon.content_length = "Content-Length: ";
        respon.content_length_v = "0";
        respon.end = "\r\n\r\n";
        
        char* buff = malloc(sizeof(char)*MAXBUFFSIZE);
        int realsize = snprintf(buff, (MAXBUFFSIZE-1), "%s%s%s%s%s", respon.status, respon.connection, respon.content_length, respon.content_length_v, respon.end);
        send(con_id, buff, realsize, 0);
        free(buff);
    }
    else{

    }
}

int rcv_handler(int con_id)
{
    char *stopword = "\r\n";
    char *pch, name, path;
    int tempsize = 0;

    struct http_request *req;  

    char startline[URLSIZE];
    char lenline[URLSIZE];
    char filenameline[URLSIZE];
    int method;
    size_t ret;
    char rcv[MAXBUFFSIZE]={};

    req = malloc(sizeof(struct http_request));

    ret = recv(con_id, rcv, MAXBUFFSIZE, 0);
    total_request_counter += 1;
    if(ret <= 0){
        LOG_INFO("recevie null request! \n");
        return 0;
    }
    else{
        LOG_INFO("get new request, total request %d\n", total_request_counter);
        LOG_DBG("Get request:%s\n",rcv);
    }
    
    size_t location = strcspn(rcv, stopword);
    
    req->startline = rcv;
    req->startline_len = location;
    req->header = rcv+location+2;

    memcpy(startline, rcv, req->startline_len); /*keep the first line of request*/
    startline[req->startline_len]='\0'; 

   
    LOG_DBG("startline: %s\n", startline);

    if(strncmp(startline, "GET", 3) == 0){
        char *startline_arr[3]; /*Method/URL/VERSION*/
        FILE *fp;
        char buffer[MAXBUFFSIZE];
        char wholename [URLSIZE];
        long filesize = 0;
        
        split(startline_arr, startline , " ");
        LOG_DBG("get url: %s\n", startline_arr[1]);
        snprintf(wholename, URLSIZE, "%s%s", cwd, startline_arr[1]);
        LOG_DBG("path: %s\n", wholename);

        if((fp = fopen(wholename, "r")) != NULL){
            LOG_DBG("open file!! \n");
            /*using file end and file head to get file size*/
            fseek(fp, 0, SEEK_END);
            filesize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            /*get file size end*/
            LOG_DBG("filesize: %ld\n", filesize);

            send_response(con_id, 200, filesize);
            if(MAXBUFFSIZE > filesize){
                ret = fread(buffer, filesize, 1, fp);
                int check = send(con_id, buffer, filesize, 0);
                LOG_DBG("check send value %d\n", check);
                LOG_DBG("send data packet! packet filesize size:%ld \n", filesize);
            }
            else{
                int temp = filesize;
                ret = fread(buffer, MAXBUFFSIZE, 1, fp);
                send(con_id, buffer, MAXBUFFSIZE, MSG_MORE);
                temp = temp - MAXBUFFSIZE;
                while(temp>0 && ret>0){

                    if(temp > MAXBUFFSIZE){
                        ret = fread(buffer, MAXBUFFSIZE, 1, fp);
                        send(con_id, buffer, MAXBUFFSIZE, MSG_MORE);
                        LOG_DBG("send data packet! temp size %d packet size:%d \n",temp ,MAXBUFFSIZE);
                    }
                    else if(temp <= MAXBUFFSIZE){
                        ret = fread(buffer, temp, 1, fp);
                        send(con_id, buffer, temp, 0);
                        LOG_DBG("send data packet! packet temp size:%d \n", temp);
                    }
                    else{
                        //pass
                    }
                    temp = temp - MAXBUFFSIZE;
                }
            }
            fclose(fp);
        }
        else{
            filesize = 0;
            LOG_INFO("cann't find file or cann't open file\n");
            send_response(con_id, 404, filesize);
            return 1;
        }
    }
    else if(strncmp(startline, "POST", 4)==0){
        char *startline_arr[3];
        struct http_post post_info;
                
        split(startline_arr, startline , " ");
        LOG_DBG("get url: %s\n", startline_arr[1]); 
        char * wholename = malloc(strlen(cwd) + strlen(startline_arr[1]));
        sprintf(wholename, "%s%s", cwd, startline_arr[1]);
        LOG_DBG("path: %s\n", wholename);

        struct header_s *header = NULL, *last = NULL;
        char* temp = req->header;
        while(temp[0]!='\r' || temp[1]!='\n'){
            last = header;
            header = malloc(sizeof(header_t));
            if(!header){
                LOG_ERR("malloc error !\n");
                return 0;
            }

            /*header content name*/
            size_t name_len = strcspn(temp, ":");
            header->name = malloc(name_len + 1);
            if(!header->name){

                LOG_ERR("malloc error !\n");
                return 0;
            }
            memcpy(header->name, temp, name_len);
            header->name[name_len] = '\0';
            
            temp += name_len + 1; // move past :
            while (*temp == ' ') {
                temp++;
            }

            /*header content value*/
            size_t value_len = strcspn(temp, "\r\n");
            header->value = malloc(value_len + 1);
            if (!header->value) {
                
                LOG_ERR("malloc error !\n");
                return 0;
            }
            memcpy(header->value, temp, value_len);
            header->value[value_len] = '\0';

            LOG_DBG("%s : %s\n", header->name, header->value);

            /*store specific content value*/
            if(strncmp(header->name, "Content-Length", 14)==0){
                post_info.content_len = atoi(header->value);
                LOG_DBG("content_len=%d\n", post_info.content_len);
            }
            else if(strncmp(header->name, "Content-Type", 12)==0){
                if(strncmp(header->value, "multipart", 9)==0){
                    post_info.multipart = 1;
                    size_t len = strcspn(header->value, "=");
                    LOG_DBG("len %d\n", len);
                    post_info.boundary = malloc(value_len-len+1);
                    memcpy(post_info.boundary, temp+len+1, value_len-len);
                    LOG_DBG("post_info.boundary: %s\n", post_info.boundary);
                    
                    /* send 404 back don't support mulitpart*/
                    send_response(con_id, 415, 0);
                    return 1;
                }
                else{
                    post_info.multipart = 0;
                    if(strncmp(header->value, "text/html", 9)==0){
                        post_info.content_type = html;
                    }
                    else if(strncmp(header->value, "application/json", 16)==0){
                        post_info.content_type = json;
                    }
                    else{
                        post_info.content_type = not_support;
                        send_response(con_id, 415, 0);
                        return 1;
                    }
                }
            }
            temp += value_len + 2; // move past <CR><LF>
            // next
            header->next = last;
        }
        /* check header ending */
        if(strncmp(temp, "\r\n", 2)==0)
        {
            if(strlen(temp)==2){
                ret = recv(con_id, rcv, MAXBUFFSIZE, 0);

                temp = rcv;
                if(ret <= 0){
                    LOG_INFO("recevie null request! \n");
                    return 0;
                }
                else{
                    LOG_INFO("get new request, total request %d\n", total_request_counter);
                    LOG_DBG("Get request:%s\n",rcv);
                }
            }
            else{
                temp+=2;
            }
            
            FILE *fp;
            switch (post_info.content_type)
            {
                case html:
                    strncat(wholename, ".html", 5);
                    LOG_DBG("wholepath: %s\n", wholename);
                    break;
                case json:
                    strncat(wholename, ".json", 5);
                    LOG_DBG("wholepath: %s\n", wholename);
                    break;
            
                default:
                    break;
            }
            if((fp = fopen(wholename, "w")) != NULL){
                fwrite(temp, sizeof(char), post_info.content_len, fp);
                fclose(fp);
                send_response(con_id, 200, 0);
            }
            else{
                LOG_ERR("open file error!\n");
            }
            
            // req->payload = malloc(post_info.content_len+1);
            // memcpy(req->payload, temp, post_info.content_len);
            // req->payload[post_info.content_len]='\0';
            // LOG_DBG("payload: %s\n", req->payload);          
        }
        else{
            send_response(con_id, 404, 0);
        }
        
    }
    else{
        send_response(con_id, 404, 0);
    }
    return 1;
}

int main(int argc , char *argv[])
{
    int socketR = 0, clientR, on =1;
    int backlogNum = 5;
    struct sockaddr_in server_info, client_info;
    int addrlen = sizeof(client_info);

    total_connection_counter = 0;
    total_request_counter = 0;
    total_response_counter = 0;
    current_connction_counter = 0;

    if(getcwd(cwd, sizeof(cwd)) != NULL){
        LOG_DBG("Current working dir: %s\n", cwd);
    }else{
        LOG_ERR("getcwd() error");
    }

    //AF_INET=IPv4 AF_INET6=IPv6, SOCK_STREAM=connection SOCK_DGRAM=connectionless
    socketR = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socketR == -1){
        LOG_ERR("fail to create a socket! \n");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(socketR, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
        LOG_ERR("set socket address resuse error! \n");
        exit(EXIT_FAILURE);
    }

    bzero(&server_info, sizeof(server_info)); //init set info all bit to zero
    server_info.sin_family  = AF_INET;
    server_info.sin_port = htons(80);
    server_info.sin_addr.s_addr = INADDR_ANY; //localIP decided by kernal

    if(bind(socketR, (struct sockaddr*)&server_info, sizeof(server_info)) < 0){
        LOG_ERR("bind error! \n");
        exit(EXIT_FAILURE);
    }
    if(listen(socketR, backlogNum)<0){
        LOG_ERR("listen error! \n");
        exit(EXIT_FAILURE);
    }

    while(1){

        clientR = accept(socketR, (struct sockaddr*)&client_info, &addrlen);
        total_connection_counter += 1;
        current_connction_counter += 1;
        LOG_INFO("accept a new connection! total: %d, current: %d\n", total_connection_counter, current_connction_counter);
        int result = rcv_handler(clientR);
        if(result == 0){
            LOG_ERR("rcv_handler error!\n");
        }
        close(clientR);
        current_connction_counter = current_connction_counter - 1;
        LOG_INFO("close connection, current: %d, total: %d\n", current_connction_counter, total_connection_counter);
    }

    return 0;
}
