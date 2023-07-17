import can


def receive_can_messages():
    bus = can.interface.Bus(channel="can0", bustype="socketcan")

    while True:
        message = bus.recv()
        print(f"Received message: {message}")


if __name__ == "__main__":
    receive_can_messages()
