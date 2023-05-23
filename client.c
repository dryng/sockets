/* Client to connect to radio sever */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVER "localhost"
#define PORT 3500
#define BLEN 1024

#define S_(x) #x
#define S(x) S_(x)

int stream_genre(int socketd, char* read_buf)
{
  int read_len;

  while (1) {
    memset(read_buf, 0, BLEN);
    if ((read_len = read(socketd, read_buf, BLEN)) == -1) 
    {
      perror("Error reading from server socket");
      exit(1);
    }
    if (read_len > 0) {
      printf("%s\n", read_buf);
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct hostent *server_hostname;
  int socketd;
  char read_buf[BLEN];
  char write_buf[BLEN];

  server_hostname = gethostbyname(SERVER);
  server.sin_family = AF_INET;
  server.sin_port = htons((u_short) PORT);
  memcpy(&server.sin_addr, server_hostname->h_addr, server_hostname->h_length);

  socketd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (connect(socketd, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == -1) 
  {
    perror("Error connecting to server");
    exit(1);
  }

  printf("Welcome to Danny's Radio.\nWhat genre would you like to listen to? ");
  scanf("%" S(BLEN) "s", write_buf);

  if (send(socketd, write_buf, strlen(write_buf), 0) == -1)
  {
    perror("Error sending to server");
    exit(1);
  }

  stream_genre(socketd, read_buf);

  close(socketd);
  return 0;

}
