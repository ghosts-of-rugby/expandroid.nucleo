import json
import socket
import time

NUCLEO_IP = "192.168.137.131"
PORT = 1235


def start_client():
    start_time = time.time()

    socket_cl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    socket_cl.bind(("0.0.0.0", PORT))

    socket_cl.sendto("SYN".encode(), (NUCLEO_IP, PORT))  # Initilize the connection

    print(f"time: {time.time() - start_time:>６.2f}, send: syn")

    tx_data = {"led2": "on", "led3": "off"}
    socket_cl.sendto(json.dumps(tx_data).encode(), (NUCLEO_IP, PORT))

    time.sleep(0.1)

    try:
        while True:
            tx_data = {"led2": "toggle", "led3": "toggle"}
            socket_cl.sendto(json.dumps(tx_data).encode(), (NUCLEO_IP, PORT))
            print(f"time: {time.time() - start_time:>６.2f}, send: {tx_data}")

            time.sleep(0.2)

    except KeyboardInterrupt:
        print("KeyboardInterrupt")
        tx_data = {"led2": "off", "led3": "off"}
        socket_cl.sendto(json.dumps(tx_data).encode(), (NUCLEO_IP, PORT))


if __name__ == "__main__":
    start_client()
