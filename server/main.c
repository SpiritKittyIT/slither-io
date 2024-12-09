#include <stdio.h>

#include "../utils/srvlist.h"
#include "../utils/socket.h"

int main() {
	int port = -1;
	int sockfd = -1;

	if (!bind_socket(&sockfd, &port)) {
		printf("Failed to bind to any available port\n");
		return 1;
	}

	printf("Bound to port %d\n", port);

	pid_t pid = getpid();
	printf("Server process id: %d\n", pid);

	Server server_info;
	server_info.pid = pid;
	server_info.port = port;

	if (!add_server_to_shared_memory(server_info)) {
		printf("Failed to add server to shared memory\n");
	}

	// code

	sleep(10);

	if (!read_and_print_shared_memory()) {
		fprintf(stderr, "Failed to read shared memory.\n");
	}

	// cleanup
	if (remove_server_from_shared_memory(server_info)) {
		printf("Server entry successfully removed.\n");
	} else {
		fprintf(stderr, "Failed to remove server entry.\n");
	}

	unbind_socket(&sockfd);

	return 0;
}
