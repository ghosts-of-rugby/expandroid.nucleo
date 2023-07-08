import socket
import time

IPADDR = "192.168.137.1"
PORT = 1234

sock_sv = socket.socket(socket.AF_INET)
sock_sv.bind((IPADDR, PORT))
sock_sv.listen()

# クライアントの接続受付
sock_cl, addr = sock_sv.accept()

s = time.time()

while True:
    # データ受信
    data = sock_cl.recv(1024)
    elapsed = time.time() - s
    decoded = data.decode("utf-8")
    print(f"Time: {elapsed}, Received: {decoded}")
