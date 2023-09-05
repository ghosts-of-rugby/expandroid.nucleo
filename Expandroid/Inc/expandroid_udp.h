#ifndef __EXPANDROID_UDP_H__
#define __EXPANDROID_UDP_H__

#include <stdbool.h>

#include "cJSON/cJSON.h"
#include "lwip/api.h"
#include "lwip/sockets.h"

#define SOCKET_BLOCKING 1
#define SOCKET_NONBLOCKING 0

// Server Infos
typedef struct __server_info {
  int port;
  int socket;
  uint8_t is_established;
  struct sockaddr_in client_address;
  socklen_t client_address_len;
  char *msg_buffer;
  int msg_buffer_size;
  int recv_data_size;
} server_info;

// Receive Data Type
typedef enum __recv_dtype {
  RECV_SYN,
  RECV_DATA,
  RECV_NONE,
} recv_dtype;

int create_udp_server(uint16_t port, int *sock,
                      struct sockaddr_in *client_address,
                      socklen_t *client_address_len, int blocking);

int create_udp(server_info *server, uint16_t port, char *buffer,
               int buffer_size);

recv_dtype recv_udp(server_info *server);

int nonblocking_udp(server_info *server);

void send_udp(server_info *server, char *buffer, int buffer_size);

#endif  // __EXPANDROID_UDP_H__