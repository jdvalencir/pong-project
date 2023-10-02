from oasis_protocol_codes import *

class TelePongControllers():
    def __init__(self, protocol):
        self.protocol = protocol

    def create_room_controller(self):
        message_json = self.protocol.create_room_protocol()
        if message_json["type"] == CREATE_ROOM_SUCCESS:
            room_code = message_json["payload"]
            return room_code, True
        else:
            return None, False
    
    def join_room_controller(self, room_code):
        message_json = self.protocol.join_room_protocol(room_code)
        if message_json["type"] == JOIN_ROOM_SUCCESS:
            room_code = message_json["payload"]
            return room_code, True
        else:
            return None, False
        
    def wait_for_player2_controller(self):
        message_json = self.protocol.wait_for_player2_protocol()
        if message_json["type"] == JOIN_ROOM_SUCCESS:
            return message_json["payload"], True
        elif message_json["type"] in [DISCONNECT_SUCCESS]:
            return None, False
        
    def handle_disconnect_controller(self):
        message_json = self.protocol.handle_disconnect_protocol()
        return message_json
        
    def leave_room_controller(self):
        message_json = self.protocol.leave_room_protocol()
        if message_json["type"] == LEAVE_ROOM_SUCCESS:
            return True
        else:
            return None, False

    def disconnect_controller(self):
        message_json = self.protocol.disconnect_protocol()
        if message_json["type"] == DISCONNECT_SUCCESS:
            return True
        else:
            return None, False