# http web server implement

###### tags: `brainstorming`

Hi Explorers,
As mentioned earlier, we ask each of you to implement the web http server.  That will have:

1. One or few (less than 3) web static pages.
2. Implemented in C
3. Use BSD socket APIs to receive requests and response static pages.
4. Able to parsing incoming HTTP requests and response HTTP messages
5. Implement proper code for
    A.logging server status/events
    B.instrumentations / counters
6. Extended objectives of implementing capabilities to
    A.Upload and download files,
    B.Leverage multithreading,
    C.and enhanced TCP performance.


## structure


![](https://i.imgur.com/fj10fzH.png)

## scope
* Method:
    * GET
    * POST(for upload file)
* status code:
    * 200 OK
    * 404 Not found
* log: write to file or show on screen
    * none
    * error
    * warning
    * info
    * debug
## some definition
```
/*error code*/
#define SVR_CWD_ER
#define SVR_SKT_CTE
#define SVR_SKT_BLD
#define SVR_SKT_LISN
#define SVR_SKT_CON_FULL
#define IO_FLE_OPEN

/*counter*/
int total_connection_counter
int total_request_counter
int total_response_counter
int current_connction_counter
int open_thread_counter

/*MAXSIZE*/
#define MAXTHREAD
#define MAXBUFFSIZE
#define MAX_CON

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

extensions[] =
{
    {"gif", "image/gif"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"txt",  "text/plain"},
    {0, 0}
};

/*function*/
int main(int argc , char *argv[])
void thread_handler()
void rcv_handler(char* rcv, int con_id)
void send_response(int con_id, int status, int packetsize)

...

```

# schedule
w1(9/30~10/4): can receive simple request and send response in single thread

w2(10/7~10/9): web log, web counter(connection,request and response count)

w3(10/14~10/18): can upload file/download file, can accept more than one connection //still not sure using which way (pthread? epoll?)

w4(10/21~10/24): complete multi-thread and performance enhance
# run code
show log on screen
```
$./server
```
write log in file
```
$./server > test.txt 2>&1
```
# testing

1. simple request ask index.html
```
$curl 127.0.0.1/data/index.html
```
result
  ![](https://i.imgur.com/q66O5eR.png)

2. simple request ask big html file
```
$curl 127.0.0.1/data/google.html -v
```
result
![](https://i.imgur.com/TA2E61u.png)

3. test 404 not found
```
$curl 127.0.0.1/gooo.html -v
```
![](https://i.imgur.com/wdfQojJ.png)





---
# each week status

## w1(9/30~10/4)

1. study format of http
2. study some reference of socket
3. write code using socket to create connection
4. can recevie http request and response simple 200 and 404 packet

## w2(10/7~10/9)

1. write document
2. study some way to do multi-thread
3. add counter and log to log server status/events
![](https://i.imgur.com/9htYkkz.png)
3. fix small bug when send big data