import socket
import time

NUCLEO_IP = "192.168.137.131"
PORT = 1234


def start_clent():
    start_time = time.time()

    socket_cl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    socket_cl.bind(("0.0.0.0", PORT))

    socket_cl.sendto("SYN".encode(), (NUCLEO_IP, PORT))  # Initilize the connection

    try:
        print("start")
        while True:
            rx_data, _ = socket_cl.recvfrom(1024)
            print(f"time: {time.time() - start_time:>６.2f}, recv: {rx_data.decode()}")

    except KeyboardInterrupt:
        print("KeyboardInterrupt")


if __name__ == "__main__":
    start_clent()
