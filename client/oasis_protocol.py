import json
from oasis_protocol_codes import *

class Oasis():
    def __init__(self, client_socket):
        self.client_socket = client_socket

    def handle_response_protocol(client_socket): 
        try:
            message = client_socket.recv(2048).decode()
            message_json = json.loads(message)
            print(message_json)
            return message_json
        except:
            message_json = { "type": DISCONNECT_SUCCESS, "payload": "Server disconnected" }
            return message_json
        
    def create_room_protocol(self):
        crete_room_message = { "type": CREATE_ROOM } 
        self.client_socket.send((json.dumps(crete_room_message)).encode())
        message_json = Oasis.handle_response_protocol(self.client_socket)
        return message_json

    def join_room_protocol(self, room_code):
        join_room_message = { "type": JOIN_ROOM, "payload": room_code }
        self.client_socket.send((json.dumps(join_room_message)).encode())
        message_json = Oasis.handle_response_protocol(self.client_socket)
        return message_json
    
    def leave_room_protocol(self):
        leave_room_message = { "type": LEAVE_ROOM } 
        self.client_socket.send((json.dumps(leave_room_message)).encode())
        message_json = Oasis.handle_response_protocol(self.client_socket)
        return message_json

    def disconnect_protocol(self):
        disconnect_message = { "type": DISCONNECT } 
        self.client_socket.send((json.dumps(disconnect_message)).encode())
        message_json = Oasis.handle_response_protocol(self.client_socket)
        self.client_socket.close()
        return message_json

    def wait_for_player2_protocol(self):
        while True:
            message_json = Oasis.handle_response_protocol(self.client_socket)
            if message_json["type"] == JOIN_ROOM_SUCCESS:
                return message_json
            elif(message_json["type"] == DISCONNECT_SUCCESS):
                self.client_socket.close()
                return message_json
            else:
                continue

    def handle_disconnect_protocol(self):
        while True:
            message_json = Oasis.handle_response_protocol(self.client_socket)
            if message_json["type"] == DISCONNECT_SUCCESS:
                return message_json
            elif message_json["type"] == PLAYER1_DISCONNECTED:
                return message_json
            elif message_json["type"] == PLAYER2_DISCONNECTED:
                return message_json
            else:
                continue    