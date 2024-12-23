#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "../utils/srvlist.h"
#include "../utils/socket.h"
#include "../utils/map.h"
#include "../utils/shrmem.h"
#include "srvflags.h"

void *start_handle_clients(void *arg) {
	int sockfd = *(int*)arg;

	Message message;
	while (true) {
		if (!receive_message(sockfd, &message)) {
			break;
		}
		if (message.instruction == IST_QUIT) {
			break;
		}

		printf("received message: %d by %d\n", message.instruction, message.pid);

		//if (!read_and_print_shared_memory()) {
		//	fprintf(stderr, "Failed to read shared memory.\n");
		//}
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	int size;
	bool with_obstacles;
	const char *from_file;

	// Handle flags
	if (!handle_flags(argc, argv, &size, &with_obstacles, &from_file)) {
		return 1;
	}

	pid_t pid = getpid();
	printf("Server process id: %d\n", pid);

	MapState *map_state = shrmem_create(pid, size, with_obstacles, from_file);
	if (!map_state) {
		fprintf(stderr, "Failed to create shared memory\n");
		return 1;
	}

	int port = -1;
	int sockfd = -1;

	if (!bind_socket(&sockfd, &port)) {
		printf("Failed to bind to any available port\n");
		return 1;
	}

	printf("Bound to port %d\n", port);


	Server server_info;
	server_info.pid = pid;
	server_info.port = port;

	if (!add_server_to_shared_memory(server_info)) {
		printf("Failed to add server to shared memory\n");
	}

	// code

	/*pthread_t thread;
	pthread_create(&thread, NULL, start_handle_clients, &sockfd);

	pthread_join(thread, NULL);*/
	sleep(10);
	Map *map = shrmem_get_map_init(map_state);
	map_print(map);

	// cleanup
	shrmem_destroy(pid);
	map_destroy(map);

	if (remove_server_from_shared_memory(server_info)) {
		printf("Server entry successfully removed.\n");
	} else {
		fprintf(stderr, "Failed to remove server entry.\n");
	}

	unbind_socket(&sockfd);

	return 0;
}
