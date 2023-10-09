import socket
import json
from oasis_protocol_codes import *

# Protocol class "Oasis"
class Oasis():    
    def __init__(self, server_ip, server_port):
        self.server_ip = server_ip
        self.server_port = server_port
        self.client_socket = None
        self.is_connected = False

    # Connect to server function
    def connect_to_server_protocol(self):
        try: 
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((self.server_ip, self.server_port))
            return True
        except Exception as e:
            print(f"Error connection to server {str(e)}") 
            return False, None
    
    # Send message to server function
    def send_message_protocol(self, message):
        if self.client_socket is not None:
            try: 
                self.client_socket.send((json.dumps(message)).encode())
                return True
            except Exception as e:
                print(f"Error sending message to server {str(e)}")
                return False
        else:
            return False
        
    # receive message from server function
    def receive_message_protocol(self):
        if self.client_socket is not None:
            try:
                message_json = self.client_socket.recv(2048).decode()
                if message_json:
                    message_json = json.loads(message_json)
                    return message_json
                else:
                    return None  # No hay mensaje disponible
            except Exception as e:
                print(f"Error receiving message from server: {str(e)}")
                return None
        else:
            return None

    # Create room function
    def create_room_protocol(self, nickname):
        success = self.connect_to_server_protocol()
        if success:
            crete_room_message = { "type": CREATE_ROOM, "payload": nickname } 
            if self.send_message_protocol(crete_room_message):
                response = self.receive_message_protocol()
                print(response)
                if response["type"] == CREATE_ROOM_SUCCESS:
                    return response["payload"], True
        return None, False
    
    # Join room function        
    def join_room_protocol(self, room_code, nickname):
        success = self.connect_to_server_protocol()
        if success: 
            join_room_message = { "type": JOIN_ROOM, "payload": room_code } 
            if self.send_message_protocol(join_room_message):
                response = self.receive_message_protocol()
                if response["type"] == JOIN_ROOM_SUCCESS:
                    return True
        return False
    
    # Disconnect from server function. This function does not wait for the response
    # Because the thread has to listen all the responses from the server
    def disconnect_from_server_protocol(self):
        disconnect_message = { "type": DISCONNECT }
        if self.send_message_protocol(disconnect_message):
            return True
    
    # Ready function
    def ready_protocol(self):
        ready_message = { "type": READY }
        if self.send_message_protocol(ready_message):
            return True
        
    # Not ready function
    def not_ready_protocol(self): 
        not_ready_message = { "type": NOT_READY }
        if self.send_message_protocol(not_ready_message):
            return True
    

    # TODO MAKE THE CONTROLLER IN THE SERVER SIDE AND THE CLIENT SIDE
    # def update_game_protocol(self, game):
    #     update_game_message = { "type": UPDATE_GAME, "payload": game }
    #     if self.send_message_protocol(update_game_message):
    #         return True