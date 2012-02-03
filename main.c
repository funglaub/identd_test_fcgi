#include <fcgi_stdio.h> // FCGI library
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define IDENT_PORT 113

int query_identd(char *buffer, char *remote_addr, char *remote_port) {
  int sockfd, n;
  
  struct sockaddr_in serv_addr;
  struct hostent *server;

  const char *www_port = ",80\n";
  char *request;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) {
    perror("ERROR opening socket");
    return(-1);
  }
  
  server = gethostbyname(remote_addr);
  
  if (server == NULL) {
    perror("no such host\n");
    return(-1);
  }
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr_list[0],
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(IDENT_PORT);
  
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    return(-1);
  }
  
  /* printf("Please enter the message: "); */
  /* bzero(buffer,256); */
  /* sffgets(buffer,255,stdin); */

  request = malloc(strlen(remote_port)+strlen(www_port)+2);
  strcpy(request, remote_port);
  strcat(request, www_port);

  printf("%s\n", request);
    
  n = write(sockfd,request,strlen(request));
  if (n < 0) {
    perror("ERROR writing to socket");
    return(-1);
  }
  
  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  
  if (n < 0) {
    perror("ERROR reading from socket");
    return(-1);
  }
  
  /* printf("%s\n",buffer); */
  close(sockfd);

  free(request);

  return 0;
}

int main(int argc, char *argv[])
{
  char buffer[256];
  time_t start, stop;
  
  while( FCGI_Accept() >= 0) {
    start = time(NULL);
    
    if (query_identd(&buffer[0], getenv("REMOTE_ADDR"), getenv("REMOTE_PORT")) != 0) {
      printf("Content-type: text/plain\r\n");
      printf("ooops");
      exit(0);
    }

    stop = time(NULL);
    
    printf("Content-type: text/html\r\n"
           "\r\n"
           "<html lang=\"en\">"
           "<head>"
           "<meta charset=\"utf-8\">"
           "<title>The HTML5 Herald</title>"
           "<meta name=\"description\" content=\"The HTML5 Herald\">"
           "<meta name=\"author\" content=\"SitePoint\">"
           "</head>"
           "<body>"
           "<h1>Hello %s:%s</h1>"
           "<p>Ident response: %s</p>"
           "<p>Request took %ld seconds</p>"
           "<p>Time: %s UTC</p>"
           "</body>"
           "</html>", getenv("REMOTE_ADDR"), getenv("REMOTE_PORT"), buffer, stop-start, asctime(gmtime(&stop)));
  }
  
  return 0;
}
