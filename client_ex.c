#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define net_assert(err, errmsg) { if ((err)) { perror(errmsg); assert(!(err)); } }
#define SERVER "localhost"
#define PORT 3500
#define BLEN 1024

int
main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct hostent *sp;
  int sd;
  int n;
  char buf[BLEN];

  //char* message = NULL;
  //size_t len = 0;

  /*
  if (argc != 2) {
    fprintf(stderr, "Invalid number of args. Pass 1 arg\
(in quotes) with the message to send to the server \n");
    exit(-1);
  }
  strcpy(message, argv[1]);
  */

  //sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  memset((char *) &server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons((u_short) PORT);
  sp = gethostbyname(SERVER);
  memcpy(&server.sin_addr, sp->h_addr, sp->h_length);

  //connect(sd, (struct sockaddr *) &server, sizeof(struct sockaddr_in));

  while (1) {
    char* message = NULL;
    size_t len = 0;

    printf("Enter a message to send to the server: ");
    getline(&message, &len, stdin);

    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    connect(sd, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
    
    // send a message
    send(sd, message, strlen(message), 0); 
    memset(message, 0, strlen(message));
    read(sd, message, 1024);
    printf("message from server: %s\n", message);

    /*n = recv(sd, buf, sizeof(buf), 0);
    while (n > 0) { // why loop?
      write(1, buf, n); // writing to stdout here
      n = recv(sd, buf, sizeof(buf), 0);
    }*/

    free(message);
    close(sd);
  }

  close(sd);
  exit(0);
}
