#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>

#include "socket.h"
#include "map.h"

bool bind_socket(int *sockfd, int *port) {
  *port = 49152;
  
	while (true) {
		*sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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

bool bind_client_socket(int *sockfd) {
	*sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (*sockfd < 0) {
			perror("socket");
			return false;
	}

	return true;
}

void unbind_socket(int *sockfd) {
	if (*sockfd >= 0) {
		if (close(*sockfd) == -1) {
			perror("close");
		}
	}

  *sockfd = -1;
}

bool get_server_addr(int port, struct sockaddr_in *server_addr) {
  memset(server_addr, 0, sizeof(struct sockaddr_in));
  server_addr->sin_family = AF_INET;
  server_addr->sin_port = htons(port);

  // Use loopback address for localhost
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr->sin_addr) <= 0) {
    perror("inet_pton");
    return false;
  }

  return true;
}

bool send_message(int sockfd, struct sockaddr_in *server_addr, Message *message) {
  ssize_t bytes_sent = sendto(sockfd, message, sizeof(Message), 0, 
                              (struct sockaddr *)server_addr, sizeof(struct sockaddr_in));
  if (bytes_sent < 0) {
    perror("sendto");
    return false;
  }
  return true;
}

bool receive_message(int socfd, Message *message) {
	struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

	ssize_t bytes_received = recvfrom(socfd, message, sizeof(message), 0, 
                                      (struct sockaddr *)&client_addr, &client_addr_len);
	if (bytes_received < 0) {
		perror("recv");
		return false;
	} else if (bytes_received != sizeof(Message)) {
		fprintf(stderr, "Incomplete message received.\n");
		return false;
	}

	return true;
}
