/*
* A radio
* Clients can connect to the server and 
* stream there genre of choice.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//  host info
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

// threads
#include <pthread.h>

#define PORT 3500
#define QLEN 100 
#define BLEN 1024
#define THREAD_POOL_SIZE 100

typedef enum Genre { ALT, COUNTRY, RAP, ROCK } Genre;

struct stream_genre_params {
  Genre genre;
  int client_socketd;
};

// should use a thread pool -> need a queue with a mutex (too lazy right now)
pthread_t thread_pool[THREAD_POOL_SIZE];

/* 
 * Stream the genre to the client socket descriptor
 *
 * SIGPIPE error (141) if client ctrl-c i.e dies
 *
 */
void* stream_genre(void* arg)
{
  struct stream_genre_params* sgp = (struct stream_genre_params*) arg;
  int read_len;
  char read_buf[BLEN];
  char write_buf[BLEN];

  // need to wipe buf because read fills from the begining
  // but if the message before was longer (i.e country then pop) then utry will
  // still be in the buf after pop is written
  memset(read_buf, 0, BLEN);
  if ((read_len = read(sgp->client_socketd, read_buf, BLEN)) == -1) 
  {
    perror("Error reading from client socket");
    exit(1);
  }

  if (read_len > 0) 
  {
    printf("Client genre: %s\n", read_buf);
    if (strcmp("exit", read_buf) == 0)
    {
      return 0; // remove
    }
  }

  while (1)
  {
    sleep(2);
    strcpy(write_buf, read_buf);
    if (send(sgp->client_socketd, write_buf, strlen(write_buf) + 1, 0) == -1) 
    {
      perror("Error writing to client socket");
      exit(1);
    }
  }

  if (close(sgp->client_socketd) == -1)
  {
    perror("Error closing client socket");
    exit(1);
  }

  close(sgp->client_socketd);
  printf("Closed client socket\n");
  return NULL;
}

int main(int argc, char* argv[])
{
  struct sockaddr_in self_sockaddr, client_sockaddr;
  socklen_t client_addr_len;
  int self_socketd, client_socketd;
  Genre client_genre;

  printf("Starting server...\n");

  client_addr_len = sizeof(struct sockaddr_in);

  self_sockaddr.sin_family = AF_INET;
  self_sockaddr.sin_addr.s_addr = INADDR_ANY;
  self_sockaddr.sin_port = htons((u_short) PORT);
 
  // domain: same host, type: tcp, protocol: ip
  self_socketd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  //assigns a name to an unnamed socket i.e creating the socket in the file system
  if (bind(self_socketd, (struct sockaddr*) &self_sockaddr, sizeof(struct sockaddr_in)) == -1)
  {
    perror("Error binding socket");
    exit(1);
  }

  printf("Listening for connections...\n");

  //accept incoming connections and set a queue limit for incoming connections
  if (listen(self_socketd, QLEN) == -1)
  {
    perror("Error listening for connections");
    exit(1);
  }

  while (1) 
  {
    // extracts the first connection request on the queue of pending connections
    // address is a result parameter that is filled in with the address of the
    // connecting entity, as known to the communications layer
    // address_len is amount of space pointed to be address ?? why 
    // on return its fill with actual length of address
    client_socketd = accept(self_socketd, (struct sockaddr*) &client_sockaddr, &client_addr_len);
    printf("Received connection. Passing socketd %d off.\n", client_socketd);
    struct stream_genre_params sgp = {ALT, client_socketd};
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, stream_genre, &sgp);
    //stream_genre(ALT, client_socketd, read_buf, write_buf); 
  }
  close(self_socketd);
  exit(0);
}
