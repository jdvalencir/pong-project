import socket
import struct

SERVER_IP = '127.0.0.1'  # Cambia esto a la dirección IP del servidor si es necesario
SERVER_PORT = 8080

def main():
    # Crear un socket TCP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Conectar al servidor
    client_socket.connect((SERVER_IP, SERVER_PORT))
    
    ready_signal = client_socket.recv(1024).decode()
    print("Ready_signal:", ready_signal, len(ready_signal))
    print("F o V: ", ready_signal != "READY")  # Debe ser algo como "READY"
    if ready_signal != "READY": 
        print("Error: the server is not ready")
    else: 
        while True: 
            # Enviar un mensaje al servidor (puedes personalizar esto según tu protocolo)
            message = input("What do you want to do?")
            client_socket.sendto(message.encode(), (SERVER_IP, SERVER_PORT))
            data = client_socket.recv(1024)
            # Recibir actualizaciones del estado del juego
            print("Data from server:", data.decode())

            if data.decode() == "QUIT":
                break
            # game_state = struct.unpack('iiii', data)
            # print(f"Player1Y: {game_state[0]}, Player2Y: {game_state[1]}, BallX: {game_state[2]}, BallY: {game_state[3]}")
    client_socket.close()

if __name__ == '__main__':
    main()