#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define END_MSG_CHAR '9'
#define MSG_CHAR '1'

void splash(void);
void error(char *msg);
char choice_hand(void);

int main(int argc, char *argv[])
{
  int port_num;
  int sockid;
  int sockfd;

  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);;
  
  int len;
  bool r_flag = false;
  bool w_flag = false;

  char sch, rch;
  char hand;

  srand(time(NULL));
  
  splash();
  if ( argc < 2 ) { error("Argument Error"); }
  
  port_num = atoi(argv[1]);
  printf("Listen Port: %d\n", port_num);

  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("can't open datastream socket");
  }
  
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family      = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port        = htons(port_num);

  if (bind(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    error("can't bind local address");
  }

  if (listen(sockid, 5) == -1) {
    error("can't listen");
  }
  
  if ((sockfd = accept(sockid, (struct sockaddr *)&client_addr, &client_len)) < 0) {
    error("can't accept");
  }

  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  
  while ( true ) {
    read(sockfd, &rch, 1);
    if ( rch == END_MSG_CHAR ) { break; }
    if ( rch == MSG_CHAR ) {
      sch = choice_hand();
      printf("Send: %c\n", sch);
      while (write(sockfd, &sch, sizeof(char)) != sizeof(char));
      rch = NULL;
    }
  }
  
  close(sockfd);
  close(sockid);
  
  return 0;
}

void splash(void)
{
  printf("*** Janken Child ***\n\n");  
}

void error(char *msg)
{
  puts(msg);
  exit(1);  
}

char choice_hand(void)
{
  char janken_hand[3] = {'0', '1', '2'};

  return janken_hand[rand() % 3];
}
