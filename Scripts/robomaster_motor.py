import socket
import json
import time
import matplotlib.pyplot as plt

NUCLEO_IP = "192.168.137.131"
PORT = 1236

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", PORT))
sock.sendto("SYN".encode(), (NUCLEO_IP, PORT))  # Initilize the connection

ack = sock.recv(1024)

if ack.decode() == "ACK":
    print("Connection established")


def send_state_command(ref_ang: float, ref_spd: float):
    res_ang_val = int(ref_ang * 8.5 * 10**5)
    ref_spd_val = int(ref_spd * 4000 * 8.5 / 5.3)

    tx_data = [
        {"id": 1, "name": "ref_state", "value": [res_ang_val, ref_spd_val]},
    ]
    sock.sendto(json.dumps(tx_data, separators=(",", ":")).encode(), (NUCLEO_IP, PORT))


def send_current_command(ref_cur: float):
    ref_cur_val = int(ref_cur * 16384.0 / 20.0)

    tx_data = [
        {"id": 2, "name": "ref_cur", "value": ref_cur_val},
    ]
    sock.sendto(json.dumps(tx_data, separators=(",", ":")).encode(), (NUCLEO_IP, PORT))


def send_speed_command(ref_spd: float):
    ref_spd_val = int(ref_spd)

    tx_data = [
        {"id": 2, "name": "ref_spd", "value": ref_spd_val},
    ]
    sock.sendto(json.dumps(tx_data, separators=(",", ":")).encode(), (NUCLEO_IP, PORT))


def send_angle_command(ref_ang: float):
    ref_ang_val = int(ref_ang * 7.0 * 10**5)

    tx_data = [
        {"id": 2, "name": "ref_ang", "value": ref_ang_val},
    ]
    sock.sendto(json.dumps(tx_data, separators=(",", ":")).encode(), (NUCLEO_IP, PORT))


start = time.time()

i = 0

times = []
speeds = []
angles = []
integrated_angles = []

ref_ang = 0.6
ref_spd = 0.4

integrated_angle = 0.0

prev_time = time.time()

try:
    while True:
        msg = sock.recv(1024)
        msg = json.loads(msg.decode())

        cur_time = time.time()
        elapsed = cur_time - start
        period = cur_time - prev_time
        integrated_angle += msg[1]["spd"] * 536000 / 3900 * period
        prev_time = cur_time

        # send_state_command(ref_spd * elapsed, ref_spd)
        # send_current_command(2.0)
        # send_speed_command(1000.0)

        times.append(elapsed)
        speeds.append(msg[1]["spd"])
        angles.append(msg[1]["ang"])
        integrated_angles.append(integrated_angle)

        # if i % 10 == 0:
        #     print(
        #         msg[1]["cur"] * 20 / 16384,
        #         msg[1]["spd"],
        #         msg[1]["ang"] / (7.0 * 10**5),
        #     )

        # if elapsed < 3.0:
        #     send_angle_command(0.7)
        # elif elapsed < 6.0:
        #     send_angle_command(0.0)
        # elif elapsed < 9.0:
        #     send_angle_command(-0.7)
        # else:
        #     send_angle_command(0.0)
    # plt.plot(times, speeds)
    # plt.show()

except KeyboardInterrupt:
    print(integrated_angle, msg[1]["ang"])
    plt.plot(times, angles)
    plt.plot(times, integrated_angles)
    plt.show()
