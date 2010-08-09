#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "61617"
#define	MAXBUFLEN 255

void* get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

int main (int argc, char const* argv[]) {
  printf("6SwitchD startup.\r\n");
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  struct sockaddr_storage their_addr;
  char buf[MAXBUFLEN];
  socklen_t addr_len;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_DGRAM;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0 ){
	fprintf(stderr, "getaddrinfo: %s\r\n", gai_strerror(rv));
  }
  
  // Bind to the first available socket
  for (p=servinfo; p!=NULL; p=p->ai_next) {
	if ((sockfd = socket( p->ai_family, 
						  p->ai_socktype, 
						  p->ai_protocol)) == -1) {
	  perror("6switchd: socket");
	  continue;
	}
	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	  close(sockfd);
	  perror("6switchd: bind");
	  continue;
	}
	break;
  }

  if (p == NULL) {
	fprintf(stderr, "6switchd: failed to bind socket\r\n");
	return 2;
  }

  freeaddrinfo(servinfo);

  printf("6switchd: waiting to recvfrom.\r\n");

  addr_len = sizeof(their_addr);
  if (( numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0,
		  (struct sockaddr*)&their_addr, &addr_len)) == -1) {
	perror("recvfrom");
	exit(1);
  }

  printf("6switchd: got packet from %s\r\n",
	  inet_ntop(their_addr.ss_family,
		  get_in_addr((struct sockaddr*)&their_addr),
		  s, sizeof(s)));
  printf("6switchd: packet is %d bytes long\r\n", numbytes);
  buf[numbytes]='\0';
  printf("6switchd: packet contains \"%s\"\r\n", buf);

  close(sockfd);
  return 0;

  return 0;
}

