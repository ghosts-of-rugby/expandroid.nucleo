import can
import logging
import time
import argparse


logger = logging.getLogger(__name__)


def recv_can_messages():
    bus = can.interface.Bus(channel="can0", bustype="socketcan")
    while True:
        message = bus.recv()
        print(f"Id: {message.arbitration_id}, Message: {message.data}, DLC: {message.dlc}")


def split_into_8bits(value):
    if value < 0 or value > 0xFFFFFFFF:
        raise ValueError("Value must be a 32-bit unsigned integer")

    return [(value >> (8 * i)) & 0xFF for i in range(3, -1, -1)]


def combine_into_32bits(parts: list) -> int:
    """Combine 4 8-bit unsigned integers into a 32-bit unsigned integer

    Args:
        parts (list): A list of 4 8-bit unsigned integers

    Returns:
        int: A 32-bit unsigned integer

    Raises:
        ValueError: If the length of the list is not 4
        ValueError: If any of the parts are not 8-bit unsigned integers

    """
    if len(parts) != 4:
        raise ValueError("There must be exactly 4 parts")

    value = 0
    for i, part in enumerate(parts[::-1]):
        if part < 0 or part > 0xFF:
            raise ValueError("Each part must be an 8-bit unsigned integer")
        value |= part << (8 * i)

    return value


def stream_messages():
    # create log file
    log_file = open("can_log.txt", "w")

    bus = can.interface.Bus(channel="can0", bustype="socketcan")
    logger.info("Bus is up and running")

    input("Press enter to send a message...")

    t = 0

    while True:
        tx_message = can.Message(
            arbitration_id=10,
            data=[1, 2, 3, 4, 5, 6, 7, 8],
            is_extended_id=False,
            dlc=8,
        )
        bus.send(tx_message)
        rx_message = bus.recv(timeout=0.0)

        log_file.write(f"[Tx Message] {tx_message}\n")
        log_file.write(f"[Rx Message] {rx_message}\n")

        t += 1
        time.sleep(0.1)


def echo_back():
    bus = can.interface.Bus(channel="can0", bustype="socketcan")
    while True:
        # get size of rx queue
        print(f"Rx queue size: {bus.channel_info}")

        rx_message = bus.recv(timeout=0.0)
        print(f"Received message: {rx_message}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--recv", action="store_true")
    parser.add_argument("--strm", action="store_true")
    parser.add_argument("--echo", action="store_true")
    args = parser.parse_args()

    if args.recv:
        recv_can_messages()
    elif args.strm:
        stream_messages()
    elif args.echo:
        echo_back()


if __name__ == "__main__":
    main()
