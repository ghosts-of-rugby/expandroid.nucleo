#include "connections.h"

#include "cJSON/cJSON.h"

// struct netconn *conn_button;  // connection for USER_Btn

int sock_button = -1;
int sock_led = -1;

// struct udp_pcb *pcb_button = NULL;
// ip4_addr_t dst_addr;

typedef struct Connection {
  struct netconn *conn;
  cJSON *json;
  u16_t port;
} Connection;

void send_mesage(const char *json_message, const size_t json_message_len) {}

int create_udp_server(uint16_t port, int *sock, struct sockaddr_in *address,
                      struct sockaddr_in *client_address,
                      socklen_t *client_address_len) {
  char buffer[128];
  int recv_len;

  *sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  if (*sock < 0) {
    return -1;
  }

  memset(address, 0, sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_port = htons(port);
  address->sin_addr.s_addr = INADDR_ANY;

  if (lwip_bind(*sock, (struct sockaddr *)address, sizeof(*address)) < 0) {
    lwip_close(*sock);
    return -1;
  }
  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  client_address_len = sizeof(*client_address);

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    recv_len =
        lwip_recvfrom(*sock, buffer, sizeof(buffer), 0,
                      ((struct sockaddr *)client_address), client_address_len);
    if (recv_len > 0) {
      // if (strcmp(buffer, "hello") == 0) {
      //   break;
      // }
      HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

      return 0;
    }
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

    // else if (recv_len < 0) {
    //   lwip_close(*sock);
    //   return -1;
    // }
  }

  return 0;
}