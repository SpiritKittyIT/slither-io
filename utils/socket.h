#include <stdbool.h>

#define CLIENT_LIMIT 64

typedef enum {
    IST_CONNECT,
    IST_UP,
    IST_DOWN,
    IST_LEFT,
    IST_RIGHT,
    IST_PAUSE,
    IST_QUIT,
} Instruction;

typedef struct {
    pid_t pid;
    Instruction instruction;
} Message;

bool bind_socket(int *sockfd, int *port);
void unbind_socket(int *sockfd);

bool send_message(int sockfd, Message *message);
bool receive_message(int sockfd, Message *message);
