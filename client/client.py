import socket
import tkinter as tk
import json
from graphic_gui import TelePongGUI
from oasis_protocol import Oasis
from telepong_controllers import TelePongControllers

SERVER_IP = '127.0.0.1'  # Cambia esto a la dirección IP del servidor si es necesario
SERVER_PORT = 8081

def main():
    # Crear un socket TCP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Conectar al servidor
    client_socket.connect((SERVER_IP, SERVER_PORT))
    message = client_socket.recv(2048).decode()
    message_json = json.loads(message)
    print(message_json["payload"])
    
    root = tk.Tk()    
    oasis_protocol = Oasis(client_socket)
    telepong_controllers = TelePongControllers(oasis_protocol)
    app = TelePongGUI(root, telepong_controllers)
    root.mainloop()

    client_socket.close()

if __name__ == '__main__':
    main()