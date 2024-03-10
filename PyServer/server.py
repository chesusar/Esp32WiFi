import socket
 
# Set up a TCP/IP server
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
# Bind the socket to server address and port 4321
server_address = ('192.168.137.1', 4321)
tcp_socket.bind(server_address)
 
# Listen on port 4321
tcp_socket.listen(1)
 
while True:
    print("Waiting for connection")
    connection, client = tcp_socket.accept()
 
    try:
        print("Connected to client IP: {}".format(client))
        while True:
            data = connection.recv(32)
            print("Received data: {}".format(data))
 
            if not data:
                break
 
    finally:
        connection.close()