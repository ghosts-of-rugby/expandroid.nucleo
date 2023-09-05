import can
import socket
import json
import time


# initialize CAN bus
bus = can.interface.Bus(channel="can0", bustype="socketcan")

for i in range(1000):
    s = time.time()
    tx_message = can.Message(
        arbitration_id=10,
        data=[1, 2, 3, 4, 5, 6, 7, 8],
        is_extended_id=False,
        dlc=8,
    )
    bus.send(tx_message)
    rx_message = bus.recv()
    print(f"{i} : {time.time() - s:.5f}")


# # create socket
# NUCLEO_IP = "192.168.137.131"
# PORT = 1236

# socket_cl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# socket_cl.bind(("0.0.0.0", PORT))
# socket_cl.sendto("SYN".encode(), (NUCLEO_IP, PORT))

# rx_message, _ = socket_cl.recvfrom(1024)
# print(rx_message.decode())

# tx_data = {"action": "start"}
# socket_cl.sendto(json.dumps(tx_data).encode(), (NUCLEO_IP, PORT))


# print("START")


# for i in range(10000):
#     tx_message = can.Message(
#         arbitration_id=10,
#         data=[1, 2, 3, 4, 5, 6, 7, 8],
#         is_extended_id=False,
#         dlc=8,
#     )
#     bus.send(tx_message)
#     s = time.time()
#     rx_message, _ = socket_cl.recvfrom(1024)
#     print(f"{i} : {time.time() - s:.5f}")

# socket_cl.close()
# bus.shutdown()
