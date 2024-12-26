#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../utils/srvlist.h"
#include "../utils/socket.h"
#include "../utils/map.h"
#include "../utils/shrmem.h"
#include "srvflags.h"
#include "socthread.h"
#include "memthread.h"

int main(int argc, char *argv[]) {
	int size;
	int length;
	bool with_obstacles;
	const char *from_file;

	// Handle flags
	if (!srvflags_handle(argc, argv, &size, &length, &with_obstacles, &from_file)) {
		return 1;
	}

	pid_t pid = getpid();
	printf("Server process id: %d\n", pid);

	MapState *map_state = shrmem_create(pid, size, length, with_obstacles, from_file);
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

	if (listen(sockfd, 16) < 0) {
		perror("listen");
		unbind_socket(&sockfd);
		return 1;
	}

	Server server_info;
	server_info.pid = pid;
	server_info.port = port;

	if (!add_server_to_shared_memory(server_info)) {
		printf("Failed to add server to shared memory\n");
	}

	// code

	SnakeList *snake_list = snakelist_init();

	MemthreadArgs m_args;
	m_args.map_state = map_state;
	m_args.snake_list = snake_list;
	m_args.socfd = sockfd;

	SocthreadArgs s_args;
	s_args.snake_list = snake_list;
	s_args.socfd = sockfd;

	pthread_t m_thread;
	pthread_t s_thread;

	pthread_create(&m_thread, NULL, memthread_start, &m_args);
	pthread_create(&s_thread, NULL, socthread_start, &s_args);

	pthread_join(m_thread, NULL);
	pthread_join(s_thread, NULL);

	// cleanup
	snakelist_destroy(snake_list);
	shrmem_destroy(pid);

	if (remove_server_from_shared_memory(server_info)) {
		printf("Server entry successfully removed.\n");
	} else {
		fprintf(stderr, "Failed to remove server entry.\n");
	}

	unbind_socket(&sockfd);

	return 0;
}
