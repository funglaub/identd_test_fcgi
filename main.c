#include <fcgi_stdio.h> // FCGI library
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>

#define IDENT_PORT 113

int query_identd(char *buffer, char *remote_addr, char *remote_port) {
  int sockfd, n;
  
  struct sockaddr_in serv_addr;
  struct hostent *server;

  static const char *www_port = ",80\n";
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
  
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *)&serv_addr.sin_addr.s_addr,
         (char *)server->h_addr_list[0],
         server->h_length);
  serv_addr.sin_port = htons(IDENT_PORT);
  
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    return(-1);
  }

  // Request should hole SOURCE_PORT,80
  request = malloc(strlen(remote_port)+strlen(www_port)+2);
  strcpy(request, remote_port);
  strcat(request, www_port);
  
  printf("%s\n", request);
    
  n = write(sockfd,request,strlen(request));
  if (n < 0) {
    perror("ERROR writing to socket");
    free(request);
    return(-1);
  }

  // Zero the buffer before we read the socket response into it
  memset(buffer, 0, 256);
  n = read(sockfd,buffer,255);
  
  if (n < 0) {
    perror("ERROR reading from socket");
    free(request);
    return(-1);
  }
  
  close(sockfd);

  free(request);

  return 0;
}

int main(int argc, char *argv[])
{
  char buffer[256];
  time_t curtime;
  struct timeval start, stop;
  long elapsed;
  
  char *remote_addr;
  char *remote_port;
  
  while( FCGI_Accept() >= 0) {
    gettimeofday(&start, NULL); 

    remote_addr = getenv("REMOTE_ADDR");
    remote_port = getenv("REMOTE_PORT");

    if (remote_addr == NULL || remote_port == NULL) {
      printf("No remote address or port received. Terminating.\n");
      exit(EXIT_FAILURE);
    }
    
    if (query_identd(&buffer[0], remote_addr, remote_port) != 0) {
      printf("Content-type: text/plain\r\n");
      printf("\r\nCan't connect to identd server at %s\n", remote_addr);
    } else {

      gettimeofday(&stop, NULL); 
      curtime = stop.tv_sec;
      elapsed = (stop.tv_sec-start.tv_sec)*1000000 + stop.tv_usec-start.tv_usec;
      
      printf("Content-type: text/plain\r\n"
             "\r\n"
             "Hello %s %s.\n"
             "Ident response: %s\n"
             "Request took %f seconds\n"
             "Time (UTC): %s\n", remote_addr, remote_port, buffer, (double)elapsed/1e6, asctime(gmtime(&curtime)));
    }
  }
  
  return EXIT_SUCCESS;
}
