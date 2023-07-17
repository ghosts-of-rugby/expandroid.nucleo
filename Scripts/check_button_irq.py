import datetime
import socket

# Server's IP address and port number
server_ip = "192.168.137.1"  # replace with your server's IP
server_port = 1234  # replace with your server's port

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the port
server_address = (server_ip, server_port)
sock.bind(server_address)

while True:
    print("\nwaiting to receive message")
    data, addr = sock.recvfrom(4096)

    # Get the current time
    current_time = datetime.datetime.now()

    print("received {} bytes from {} at {}".format(len(data), addr, current_time))
    print(data.decode())
