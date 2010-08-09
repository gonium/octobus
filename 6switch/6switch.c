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

#define SERVERPORT "61616"

/* Command definitions. */
#define ON		0x10
#define OFF		0x11
#define STATUS	0x12

struct switchmsg_t {
  uint16_t command;
};

void usage() {
  fprintf(stderr, "USAGE: 6switch hostname {on|off|status}\r\n");
  exit(1);
}

int main (int argc, char const* argv[]) {
  printf("6switch client startup\r\n");
  struct switchmsg_t switchmsg;

  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;

  if (argc != 3) {
	usage();
  }


  memset(&switchmsg, 0, sizeof(switchmsg));
  // determine what to do
  if (strcmp(argv[2], "on") == 0) {
	printf("sending ON command\r\n");
	switchmsg.command=htons(ON);
  } else if (strcmp(argv[2], "off") == 0) {
	printf("sending OFF command\r\n");
	switchmsg.command=htons(OFF);
  } else if (strcmp(argv[2], "status") == 0) {
	fprintf(stderr, "not implemented yet.\r\n");
	exit(2);
  } else {
	usage();
  }



  memset(&hints, 0, sizeof(hints));
  hints.ai_family=AF_INET6;
  hints.ai_socktype=SOCK_DGRAM;

  if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
  }

  // loop results & make socket
  for (p=servinfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(
			p->ai_family,
			p->ai_socktype,
			p->ai_protocol)) == -1) {
	  perror("6switch: socket");
	  continue;
	}
	break;
  }

  if (p == NULL) {
	fprintf(stderr, "6switch: failed to bind to socket.\r\n");
	return 2;
  }



  if ((numbytes = sendto(sockfd, &switchmsg,
		  sizeof(switchmsg),
		  0, p->ai_addr, p->ai_addrlen)) == -1){
	perror("6switch: sendto");
	exit(1);
  }

  freeaddrinfo(servinfo);

  printf("6switch: send %d bytes to %s.%s\r\n", numbytes, argv[1], SERVERPORT);
  close(sockfd);

  return 0;
}

