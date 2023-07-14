import socket
import time

host = "192.168.137.1"
# host = "127.0.0.1"
port = 1234

sock_sv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print(f"IP:{host}, Port:{port}")
sock_sv.bind((host, port))
sock_sv.listen(1)

# クライアントの接続受付
sock_cl, addr = sock_sv.accept()

s = time.time()

while True:
    # データ受信
    data = sock_cl.recv(1024)
    elapsed = time.time() - s
    decoded = data.decode("utf-8")
    print(f"Time: {elapsed}, Received: {decoded}")
