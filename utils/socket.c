#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"
#include "map.h"

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

bool send_message(int sockfd, Message *message) {
	if (send(sockfd, message, sizeof(Message), 0) < 0) {
		perror("send");
		return false;
	}

	return true;
}

bool receive_message(int sockfd, Message *message) {
	ssize_t bytes_received = recv(sockfd, message, sizeof(Message), 0);
	if (bytes_received < 0) {
		perror("recv");
		return false;
	} else if (bytes_received == 0) {
		printf("Client disconnected.\n");
		return false;
	} else if (bytes_received != sizeof(Message)) {
		fprintf(stderr, "Incomplete message received.\n");
		return false;
	}

	return true;
}
