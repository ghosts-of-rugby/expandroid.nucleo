#ifndef __CONNECTIONS_H
#define __CONNECTIONS_H

#include "lwip/api.h"
#include "lwip/sockets.h"

#define SERVER_IP "192.168.137.1"

#define USER_Btn_IRQ_Port 1234
#define LED_PORT 1235

extern int sock_button;  // connection for USER_Btn
extern int sock_led;     // connection for LED

// extern struct udp_pcb *pcb_button;
// extern ip4_addr_t dst_addr;

extern osEventFlagsId_t buttonPressedEventHandle;

int create_udp_server(uint16_t port, int *sock, struct sockaddr_in *address,
                      struct sockaddr_in *client_address,
                      socklen_t *client_address_len);

#endif