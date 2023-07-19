#ifndef __CONNECTIONS_H
#define __CONNECTIONS_H

#include "lwip/api.h"
#include "lwip/sockets.h"
#include "cJSON/cJSON.h"

#define SERVER_IP "192.168.137.1"

#define PORT_USER_Btn_IRQ 1234
#define PORT_LED 1235
#define PORT_CAN01 1236


extern osEventFlagsId_t buttonPressedEventHandle;

/**
 * @brief Create a UDP server and wait for a "hello" message from a client.
 *
 * This function creates a UDP server that listens on the specified port.
 * It waits for a "hello" message from a client. When it receives such a
 * message, it breaks the loop and the function returns. If any error occurs
 * during the creation of the socket or the binding process, the function
 * returns an error code.
 *
 * @param port The port on which the server will listen for incoming messages.
 * @param sock A pointer to an integer where the socket descriptor will be
 * stored.
 * @param client_address A pointer to a sockaddr_in structure where the client's
 * address will be stored.
 * @param client_address_len A pointer to a socklen_t where the length of the
 * client's address will be stored.
 *
 * @return 0 if the server was successfully created and a "hello" message was
 * received, -1 if an error occurred during the creation or binding of the
 * socket.
 */
int create_udp_server(uint16_t port, int *sock,
                      struct sockaddr_in *client_address,
                      socklen_t *client_address_len);



void control_led_from_json(cJSON *json);

#endif