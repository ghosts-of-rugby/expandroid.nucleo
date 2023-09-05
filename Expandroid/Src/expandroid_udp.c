#include "expandroid_udp.h"

int create_udp_server(uint16_t port, int *sock,
                      struct sockaddr_in *client_address,
                      socklen_t *client_address_len, int blocking) {
  char buffer[128];
  int recv_len;
  struct sockaddr_in address;

  *sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  if (*sock < 0) {
    return -1;
  }

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  if (lwip_bind(*sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
    lwip_close(*sock);
    return -1;
  }
  *client_address_len = sizeof(*client_address);

  if (!blocking) {
    // set socket to non-blocking mode
    int val = 1;
    lwip_ioctl(*sock, FIONBIO, &val);
  }

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    recv_len =
        lwip_recvfrom(*sock, buffer, sizeof(buffer), 0,
                      ((struct sockaddr *)client_address), client_address_len);
    if (recv_len > 0) {
      if (strcmp(buffer, "SYN") == 0) {
        lwip_sendto(*sock, "ACK", sizeof("ACK"), 0,
                    (struct sockaddr *)client_address, *client_address_len);
        break;
      }
      return 0;
    } else if (recv_len < 0) {
      lwip_close(*sock);
      return -1;
    }
  }

  return 0;
}

void check_syn_and_send_ack(char *buffer, int sock,
                            struct sockaddr_in *client_address,
                            socklen_t client_address_len) {
  if (strcmp(buffer, "SYN") == 0) {
    lwip_sendto(sock, "ACK", sizeof("ACK"), 0,
                (struct sockaddr *)client_address, client_address_len);
  }
}

int create_udp(server_info *server, uint16_t port, char *buffer,
               int buffer_size) {
  struct sockaddr_in address;
  server->socket = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  server->port = port;
  server->recv_data_size = 0;
  if (server->socket < 0) {
    return -1;
  }
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  if (lwip_bind(server->socket, (struct sockaddr *)&address, sizeof(address)) <
      0) {
    lwip_close(server->socket);
    return -1;
  }
  server->client_address_len = sizeof(server->client_address);

  server->msg_buffer = buffer;
  server->msg_buffer_size = buffer_size;

  server->is_established = 0;

  // Wait SYN and send ACK
  while (1) {
    int ret = recv_udp(server);
    if (ret == RECV_SYN) {
      send_udp(server, "ACK", sizeof("ACK") - 1);
      break;
    }
  }

  return 0;
}

int nonblocking_udp(server_info *server) {
  int val = 1;
  return lwip_ioctl(server->socket, FIONBIO, &val);  // set non-blocking
}

recv_dtype recv_udp(server_info *server) {
  // Clear Buffer
  memset(server->msg_buffer, '\0', server->msg_buffer_size);

  server->recv_data_size =
      lwip_recvfrom(server->socket, server->msg_buffer, server->msg_buffer_size,
                    0, ((struct sockaddr *)&server->client_address),
                    &server->client_address_len);

  if (strcmp(server->msg_buffer, "SYN") == 0) {
    return RECV_SYN;
  }

  return server->recv_data_size > 0 ? RECV_DATA : RECV_NONE;
}

void send_udp(server_info *server, char *buffer, int buffer_size) {
  lwip_sendto(server->socket, buffer, buffer_size, 0,
              (struct sockaddr *)&server->client_address,
              server->client_address_len);
}