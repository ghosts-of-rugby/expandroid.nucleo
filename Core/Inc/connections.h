#ifndef __CONNECTIONS_H
#define __CONNECTIONS_H

#include "lwip/api.h"

#define SERVER_IP "192.168.137.1"

#define USER_Btn_IRQ_Port 1234
#define LED_PORT 1235

extern int sock_button;  // connection for USER_Btn
extern int sock_led;     // connection for LED

extern osEventFlagsId_t buttonPressedEventHandle;

#endif