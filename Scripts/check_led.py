import socket
import time


NUCLEO_IP = "192.168.137.131"
LED_PORT = 1235


def start_client():
    socket_cl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    socket_cl.sendto("hello".encode(), (NUCLEO_IP, LED_PORT))
    print("start")

    while True:
        socket_cl.sendto("hello".encode(), (NUCLEO_IP, LED_PORT))
        print("send")

        time.sleep(1)


if __name__ == "__main__":
    start_client()
