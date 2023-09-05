import json
import socket
import time

NUCLEO_IP = "192.168.137.131"
PORT = 1236


socket_cl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
socket_cl.bind(("0.0.0.0", PORT))
socket_cl.sendto("SYN".encode(), (NUCLEO_IP, PORT))  # Initilize the connection

# try:
#     while True:
#         command = input("Please input command: ")
#         if " " in command:
#             command, arg = command.split(" ")

#         if command == "start":
#             print("Start CAN")
#             tx_data = {"action": "start"}

#         elif command == "send":
#             tx_data = {
#                 "action": "send",
#                 "header": {
#                     "id": 123,
#                     "is_extended_id": False,
#                     "is_remote_frame": False,
#                     "dlc": 8,
#                 },
#                 "message": [1, 2, 3, 4, 5, 6, 7, 8],
#             }
#             print(arg)
#         socket_cl.sendto(json.dumps(tx_data).encode(), (NUCLEO_IP, PORT))
#         # rx_data = json.loads(input())
#         # print(rx_data)
# except KeyboardInterrupt:
#     print("\nFinish")


tx_data = {"action": "start"}
socket_cl.sendto(json.dumps(tx_data).encode(), (NUCLEO_IP, PORT))

while True:
    message, _ = socket_cl.recvfrom(1024)
    print(message.decode())
