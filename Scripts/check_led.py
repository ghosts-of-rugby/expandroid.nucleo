import socket
import time


def start_client():
    # host = "127.0.0.1"
    host = "192.168.137.131"
    port = 1235
    socket_cl = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    socket_cl.connect((host, port))
    while True:
        # data = input("Enter data to send: ")
        socket_cl.send("hello".encode())
        time.sleep(1)

        # received_data = socket_cl.recv(1024).decode()
        # print("Received:", received_data)

        # if received_data == "messageB":
        #     print("Sending: messageA")
        #     socket_cl.send("messageA".encode())

    socket_cl.close()


if __name__ == "__main__":
    start_client()
