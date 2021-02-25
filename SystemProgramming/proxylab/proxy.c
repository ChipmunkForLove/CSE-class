/*
 * proxy.c - a simple, iterative HTTP proxy server
 */
#include "csapp.h"
#include "cache.h"

#define PROXY_LOG "proxy.log"

void doit(int fd);
void proxy_cache_log(char*, char*, int);
void print_requesthdrs(rio_t *rp);
void parse_uri_proxy(char*,char*,int*);
void clienterror(int fd, char *cause, char *errnum,char *shortmsg, char *longmsg);
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
/*
 * main: 
 *  listens for connections on the given port number, handles HTTP requests 
 *  with the doit function then closes the connection 
 */

  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  //cache initializing
  header.totallength=0;
  header.next= (cache_block*)Malloc(sizeof(cache_block));
  
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  port=atoi(argv[1]);
  /// listen for connections
  listenfd=Open_listenfd(port);

/// if a client connects, accept the connection, handle the requests
/// (call the do it function), then close the connection

  while(1){
    clientlen=sizeof(clientaddr);
    connfd=Accept(listenfd,(SA*)&clientaddr,&clientlen);
    doit(connfd);
    Close(connfd);
  }
}

//-----------------------------------------------------------------------------
void doit(int fd)
{
/*
 * doit: reads HTTP request from connection socket, forwards the request to the
 *  requested host. Reads the response from the host server, and writes the
 *  response back to the client 
 * params:
 *    - fd (int): file descriptor of the connection socket.
 */  
  char line[MAXLINE], host[MAXLINE];
  char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char responseBuffer[MAXLINE],responseHeader[MAXLINE];

  int serverfd, port=80,contentLength,addfail;
 
  cache_block* cache;
  char *cached = "uncached";
  rio_t rio,server;

  //initialize  
  memset(line, 0, MAXLINE);
  memset(host, 0, MAXLINE);
  memset(method, 0, MAXLINE);
  memset(uri, 0, MAXLINE);
  memset(version, 0, MAXLINE);
  memset(responseBuffer, 0, MAXLINE);
  memset(responseHeader, 0, MAXLINE);

  Rio_readinitb(&rio, fd);

  /// read request header
  Rio_readlineb(&rio,line,MAXLINE);
  sscanf(line,"%s %s %s",method,uri,version);

  /// get hostname, port, filename by parse_uri()
  parse_uri_proxy(uri,host,&port);

  /// check the method is GET, if it is not, return a 501 error by using clienterror()
  if(strcmp(method, "GET")) {
      clienterror(fd, method, "501", "Not implemented","This method is not implemented in proxy server");
      //fd,cause(method),errnum,shortmsg,longmsg      
      return;
   }
  /// find the URI in the proxy cache.
  /// if the URI is in the cache, send directly to the client 
  /// be sure to write the log when the proxy server send to the client  

  cache=find_cache_block(uri);
  if(cache!=NULL){
    cached="cached"; 
    proxy_cache_log(cached,uri,cache->contentLength);

    Rio_writen(fd,cache->response,strlen(cache->response));
    Rio_writen(fd,cache->content,cache->contentLength);
    return;
  }
 
  //open server and send request
  serverfd = Open_clientfd(host, port);  
  
  if(serverfd <0){
	printf("Open_clientfd failed\n");
        return;

  }

  // send request to server
  Rio_readinitb(&server,serverfd);  
  Rio_writen(serverfd,line,strlen(line)); 
 
  while(strcmp(line, "\r\n")>0) {
                Rio_readlineb(&rio, line, MAXLINE);
		Rio_writen(serverfd, line, strlen(line));
		printf("%s", line);
	} 

  /// read response header
  /// read the response header from the server and build the proxy's responseBuffer
  /// header by repeatedly adding the responseBuffer (server response)
  /// this proxy server only supports 'Content-Length' format.

  Rio_readlineb(&server,responseBuffer,MAXLINE);
  
  // append buffer to header
  strcat(responseHeader,responseBuffer);
 
  while(strcmp(responseBuffer, "\r\n")>0) {
	Rio_readlineb(&server, responseBuffer,MAXLINE);
	sscanf(responseBuffer,"Content-Length: %d", &contentLength);
	
      if(strstr(responseBuffer,"Content-length")){
        sscanf(responseBuffer,"Content-length: %d",&contentLength);
      } 
  
      strcat(responseHeader, responseBuffer);
   }

	printf("%s\n",responseHeader);

	char* toClient = (char *) Malloc(contentLength);
  
  /// Content-Lengh
  /// using the 'Content-Length' read from the http server response header,
  /// you must allocate and read that many bytes to our buffer
  /// you now write the response heading and the content back to the client
  
  Rio_readnb(&server, toClient, contentLength);
  Rio_writen(fd, responseHeader, strlen(responseHeader));
  Rio_writen(fd, toClient, contentLength);
   
  /// add the proxy cache 
  /// logging the cache status and other information  
  /// check the free or close   
  
  if(contentLength>0){
  	addfail=add_cache_block(uri,toClient,responseHeader,contentLength);
  
  	if(addfail>0){
    	cached="uncached";
    	proxy_cache_log(cached,uri,contentLength);
  	}
	free(toClient);
  	Close(serverfd);
  }
}

void proxy_cache_log(char* cached, char*uri, int contentLength){
/*
 * proxy_cache_log:
 *       keep the track of all the cache-log when add or find the contents
 *params:
    -cached: status of the cache corresponding uri
   -uri: uri string
   -contentLength: size of the content length (bytes)
 *       
 */
  char timebuf[MAXLINE];
  time_t timet;
  struct tm *timeinfo;
  char month[4],day[4];
  FILE* f;
   
  timet= time(NULL);
  timeinfo=localtime(&timet);
  
 switch(timeinfo->tm_mon){
  	case(0):
	strcpy(month,"Jan");
	break;
	case(1):
	strcpy(month,"Feb");
	break;
	case(2):
	strcpy(month,"Mar");
	break;
	case(3):
	strcpy(month,"Apr");
	break;
	case(4):
	strcpy(month,"May");
	break;
	case(5):
	strcpy(month,"Jun");
	break;
	case(6):
	strcpy(month,"Jul");
	break;
	case(7):
	strcpy(month,"Aug");
	break;
	case(8):
	strcpy(month,"Sep");
	break;
	case(9):
	strcpy(month,"Oct");
	break;
	case(10):
	strcpy(month,"Nov");
	break;
	default:
	strcpy(month,"Dec");

  }
  switch(timeinfo->tm_wday){
        case(0):
        strcpy(day,"Sun");
        break;
        case(1):
        strcpy(day,"Mon");
        break;
        case(2):
        strcpy(day,"Tue");
        break;
        case(3):
        strcpy(day,"Wen");
        break;
        case(4):
        strcpy(day,"Thu");
        break;
        case(5):
        strcpy(day,"Fri");
        break;
        default:
        strcpy(day,"Sat");
  }


	f=fopen(PROXY_LOG,"a");
       
         sprintf(timebuf,"%s %d %s %d %d:%d:%d %s: %s %d\n",day,timeinfo->tm_mday,month,(timeinfo->tm_year+1900),
                timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,timeinfo->tm_zone,uri,contentLength);

	if(!strcmp(cached,"cached")){
		fprintf(f,"[cached] %s",timebuf);
	}
	else if(!strcmp(cached,"uncached")){
		fprintf(f,"[uncached] %s",timebuf);
	}
	fclose(f);
}

void print_requesthdrs(rio_t *rp)
{
/**** DO NOT MODIFY ****/
/**** WARNING: This will read out everything remaining until a line break ****/
/*
 * print_requesthdrs: 
 *        reads out and prints all request lines sent to the server
 * params:
 *    - rp: Rio pointer for reading from file
 *
 */
  char buf[MAXLINE];
  Rio_readlineb(rp, buf, MAXLINE);
  while(strcmp(buf, "\r\n")) {
    printf("%s", buf);
    Rio_readlineb(rp, buf, MAXLINE);
  }
    printf("\n");
  return;
}

void parse_uri_proxy(char* uri, char* host, int *port){
/*
 * parse_uri_proxy:
 *        Get the hostname, port, and parse the uri.
 * params:
 *       - uri: uri string 
 *       - host: host string extracted from the uri.
 *       - port: port number
 *
 * you have to add further parsing steps in http.c 
 * 
 * example1: http://www.snu.ac.kr/index.html
 *          host: www.snu.ac.kr 
 *          filename: should be /index.html 
 *          port: 80 (default)
 *
 * example2: http://www.snu.ac.kr:1234/index.html
 *           host: www.snu.ac.kr
 *           filename: /index.html 
 *           port: 1234
 *
 * example3: http://127.0.0.1:1234/index.html
 *           host: 127.0.0.1
 *           filename: /index.html
 *           port: 1234
 *           
 *   
*/
  sscanf(uri, "http://%[^:/]:%d", host, port);
  if(!port) *port = 80;
}

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
/**** DO NOT MODIFY ****/
/*
 * clienterror: 
 *        Creates appropriate HTML error page and sends to the client 
 * params:
 *    - fd: file descriptor of connection socket.
 *    - cause: what has caused the error: the filename or the method
 *    - errnum: The HTTP status (error) code
 *    - shortmsg: The HTTP status (error) message
 *    - longmsg: A longer description of the error that will be printed 
 *           on the error page
 *
 */  
  char buf[MAXLINE], body[MAXBUF];
  ///build the HTTP response body 
  sprintf(body, "<html><title>Mini Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s<b>%s: %s</b>\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>Mini Web server</em>\r\n", body);

  /// print the HTTP response
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-Length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

