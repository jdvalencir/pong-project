import socket

SERVER_IP = '127.0.0.1'  # Cambia esto a la dirección IP del servidor si es necesario
SERVER_PORT = 8084

def main():
    # Crear un socket TCP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Conectar al servidor
    client_socket.connect((SERVER_IP, SERVER_PORT))
    
    print("============TELEPONG============")
    print("!Hi There¡ WELCOME TO TELEPONG")
    
    in_room = False  # Indica si el usuario está en una sala
    room_code = ""  # Almacena el código de la sala actual
    room = ""
    while True:
        if not in_room:
            # Solo preguntar al usuario si no está en una sala
            data = input("Please enter your option: 1. Create a room 2. to join a room 3. to exit: ")
            if data == "1" or data == "2":
                client_socket.send(data.encode())
                if data == "2":
                    room = input("Please enter the room number: ")
                    client_socket.send(room.encode())
                response = client_socket.recv(1024)
                print(response.decode())
                if response.decode().startswith("Success"):
                    in_room = True  # Cambiar a True cuando el usuario entra en una sala
                    room_code = room  # Almacena el código de la sala actual
            elif data == "3":
                print("Exiting...")
                break
            else:
                print("Invalid option. Please enter 1, 2, or 3.")
        else:
            action = input("You are in room {}. Enter 'exit' to leave the room: ".format(room_code))
            if action == "exit":
                client_socket.send("exit".encode())  # Envia una señal al servidor de que el usuario quiere salir de la sala
                response = client_socket.recv(1024)
                print(response.decode())
                in_room = False
                room_code = ""
            else:
                # Recibir mensajes del servidor sin bloquear el flujo
                client_socket.settimeout(0.1)  # Establece un tiempo de espera breve
                try:
                    response = client_socket.recv(1024)
                    print(response.decode())
                except socket.timeout:
                    pass  # Continúa sin bloquear si no hay mensajes pendientes
        
    client_socket.close()

if __name__ == '__main__':
    main()