#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define END_MSG_CHAR '9'
#define MSG_CHAR '1'

void splash(void);
void error(char *msg);
char select_hand(void);
void show_hand(char hand);
void show_result(int result);
int compare_hand(char p1, char p2);

int main(int argc, char *argv[])
{
  int port_num;
  int ipaddr;
  int sockid;
  int sockfd;
  int len;
  int result;
  char sch, rch;
  char hand;
  bool r_flag = false;
  bool w_flag = false;
  bool end_flag = false;
  struct sockaddr_in server_addr, client_addr;
  struct hostent *retrieve;
  socklen_t client_len = sizeof(client_addr);;  

  if ( argc < 3 ) { error("Argument Error"); }
  
  port_num = atoi(argv[2]);
  if ((retrieve = gethostbyname(argv[1])) == NULL) {
    error("Unknown host name");
  }

  ipaddr = *(unsigned int *)(retrieve->h_addr_list[0]);
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family      = AF_INET;
  server_addr.sin_addr.s_addr = *(unsigned int *)(retrieve->h_addr_list[0]);
  server_addr.sin_port        = htons(port_num);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("can't open datastream socket");
  }
  
  if ( connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    error("can't connect");
  }

  fcntl(sockfd, F_SETFL, O_NONBLOCK);

  while ( true ) {
    splash();

    hand = select_hand();

    if ( hand == NULL ) { continue; }
    if ( hand == '9' ) {
      sch = END_MSG_CHAR;
      write(sockfd, &sch, sizeof(char));
      break;
    }
    
    sch = MSG_CHAR;
  
    write(sockfd, &sch, sizeof(char));
    while ( read(sockfd, &rch, sizeof(char)) != sizeof(char) );

    result = compare_hand(hand, rch);
  
    printf("Player is %c\n", hand);
    printf("Computer is %c\n", rch);

    show_result(result);
  }

  close(sockfd);
  
  return 0;
}

void splash(void)
{
  printf("\n*******************\n");
  printf("*** Janken Game ***\n");
  printf("*******************\n\n");

  return;
}

void error(char *msg)
{
  puts(msg);
  exit(1);  
}

char select_hand(void)
{
  char hand;
  
  printf("Select your hand[Stone=0, Scissors=1, Paper=2, END=9]\n: ");
  scanf("%c", &hand);
  (void)getchar(); 

  if ( hand != '0' && hand != '1' && hand != '2' && hand != '9') {
    return NULL;
  }
  
  return hand;
}

void show_result(int result)
{
  switch (result) {
  case 0:
    puts("Draw"); break;
  case 1:
    puts("Player Win"); break;
  case 2:
    puts("Computer Win"); break;
  }

  return;
}

int compare_hand(char p1, char p2)
{
  if ( p1 == p2 ) { return 0; }
  if ( ((p2 - p1) + 3) % 3 == 2 ) { return 2; }
  return 1;
}

