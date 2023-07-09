#include "connections.h"

struct netconn *conn_button;  // connection for USER_Btn

void _add_time_to_message(char *message, const size_t message_len) {
  char time_str[20];
  time_t now;
  struct tm *now_tm;
  time(&now);
  now_tm = localtime(&now);
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);
  snprintf(message, message_len, "%s: %s", time_str, message);
}

void send_mesage(const char *message, const size_t message_len,
                 struct netconn *conn, const u16_t port) {}
