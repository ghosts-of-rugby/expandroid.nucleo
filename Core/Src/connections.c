#include "connections.h"

#include "cJSON/cJSON.h"

// struct netconn *conn_button;  // connection for USER_Btn

int sock_button = -1;
int sock_led = -1;

typedef struct Connection {
  struct netconn *conn;
  cJSON *json;
  u16_t port;
} Connection;

// void _add_time_to_message(char *message, const size_t message_len) {
//   char time_str[20];
//   time_t now;
//   struct tm *now_tm;
//   time(&now);
//   now_tm = localtime(&now);
//   strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);
//   snprintf(message, message_len, "%s: %s", time_str, message);
// }

void send_mesage(const char *json_message, const size_t json_message_len) {}
