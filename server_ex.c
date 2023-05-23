#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define net_assert(err, errmsg) { if ((err)) { perror(errmsg); assert(!(err)); } }
#define PORT 4897
#define QLEN 100
#define BLEN 1024

int visits;

int
main(int argc, char *argv[])
{
  struct sockaddr_in self, client;
  struct hostent *cp;
  int sd, new_socket;
  int len;
  int valread;
  char buf[BLEN];
  char client_buf[BLEN];
  char* connected_from;
  sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  memset((char *) &self, 0, sizeof(struct sockaddr_in));
  self.sin_family = AF_INET;
  self.sin_addr.s_addr = INADDR_ANY;
  self.sin_port = htons((u_short) PORT);

  bind(sd, (struct sockaddr *) &self, sizeof(struct sockaddr_in));

  listen(sd, QLEN);

  while (1) {
    len = sizeof(struct sockaddr_in);
    new_socket = accept(sd, (struct sockaddr *) &client, (socklen_t *)&len);
    valread = read(new_socket, client_buf, BLEN); 

    cp = gethostbyaddr((char *) &client.sin_addr, sizeof(struct in_addr), AF_INET);
    connected_from = (cp && cp->h_name) ? cp->h_name : inet_ntoa(client.sin_addr);

    printf("Message from client: %s via %s \n", client_buf, connected_from);

    visits++;
    sprintf(buf, "This server has been contacted %d time(s).\n", visits);
    send(new_socket, buf, strlen(buf), 0);

    close(new_socket);
  }
  exit(0);
}
