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

    def connect_to_server_protocol(self):
        try: 
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((self.server_ip, self.server_port))
            return True
        except Exception as e:
            print(f"Error connection to server {str(e)}") 
            return False, None
    
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
        
    def receive_message_protocol(self):
        if self.client_socket is not None:
            try:
                message_json = self.client_socket.recv(2048).decode()
                if message_json:
                    message_json = json.loads(message_json)
                    return message_json
                else:
                    return None
            except Exception as e:
                print(f"Error receiving message from server: {str(e)}")
                return None
        else:
            return None

    def create_room_protocol(self, nickname):
        success = self.connect_to_server_protocol()
        if success:
            crete_room_message = f'type {CREATE_ROOM} nickname {nickname}' 
            if self.send_message_protocol(crete_room_message):
                response = self.receive_message_protocol()
                print(response)
                if response["type"] == CREATE_ROOM_SUCCESS:
                    return response["room_code"], True
        return None, False
    
    def join_room_protocol(self, room_code, nickname):
        success = self.connect_to_server_protocol()
        if success: 
            join_room_message = f'type {JOIN_ROOM} nickname {nickname} room_code {room_code}' 
            if self.send_message_protocol(join_room_message):
                response = self.receive_message_protocol()
                if response["type"] == JOIN_ROOM_SUCCESS:
                    return True
        return False
    
    # Disconnect from server function. This function does not wait for the response
    def disconnect_from_server_protocol(self):
        disconnect_message = f'type {DISCONNECT}'
        if self.send_message_protocol(disconnect_message):
            return True
    
    def ready_protocol(self):
        ready_message = f'type {READY}'
        if self.send_message_protocol(ready_message):
            return True
        
    def not_ready_protocol(self): 
        not_ready_message = f'type {NOT_READY}'
        if self.send_message_protocol(not_ready_message):
            return True
    
    def move_paddle_protocol(self, move):
        move_paddle_message = f'type {UPDATE_GAME} payload {move}'
        if self.send_message_protocol(move_paddle_message):
            return True
    
    def update_ball_controller(self, ball_x):
        update_ball_message = f'type {BALL_UPDATE} ball_x {ball_x}'
        if self.send_message_protocol(update_ball_message):
            return True

    def player_scored_protocol(self, score_player):
        player_scored_message = f'type {PLAYER_SCORED} score_player {score_player}'
        if self.send_message_protocol(player_scored_message):
            return True