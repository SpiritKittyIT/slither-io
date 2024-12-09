#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"

bool bind_socket(int *sockfd, int *port) {
  *port = 49152;
  
	while (true) {
		*sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (*sockfd < 0) {
			perror("socket");
			return false;
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(*port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(*sockfd, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
			return true;
		}

		close(*sockfd);
		++(*port);
	}
	return false;
}

void unbind_socket(int *sockfd) {
	if (*sockfd >= 0) {
		if (close(*sockfd) == -1) {
			perror("close");
		}
	}

  *sockfd = -1;
}