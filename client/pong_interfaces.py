import PySimpleGUI as sg
import threading
from oasis_protocol_codes import *
import time

class MenuInterfaces(): 
    def __init__(self, Oasis): 
        self.Oasis = Oasis
        self.window = None
        sg.theme("DarkAmber")

    def create_room_interface(self):
        create_room_layout = [
            [sg.Text("Introduce your nickname:")],
            [sg.InputText(key="nickname")],
            [sg.Button("Create room", key="create_room_button")],
            [sg.Button("Principal menu", key="principal_menu_button")]
        ]
        create_room_window = sg.Window("Crear Sala", create_room_layout, finalize=True)
        while True:
            event, values = create_room_window.read()
            if event == sg.WIN_CLOSED:
                break  
            elif event == "create_room_button":
                nickname = values["nickname"]
                if nickname == "":
                    sg.popup("Invalid nickname")
                else:
                    payload, success = self.Oasis.create_room_protocol(nickname)
                    if success:
                        create_room_window.close()  
                        self.waiting_for_player_interface(payload)
            elif event == "principal_menu_button":
                create_room_window.close() 
                self.window.un_hide()
        create_room_window.close()   

    def waiting_for_player_interface(self, code_room): 
        waiting_for_player_layout = [
            [sg.Text("Waiting for player to join the room")],
            [sg.Text(f"Room code: {code_room}")],
            [sg.Button("Principal menu", key="principal_menu_button")],
            [sg.Output(size=(50, 10))]
        ]
        waiting_for_player_window = sg.Window("Waiting for player", waiting_for_player_layout, finalize=True)
        print("Waiting for player")
        while True: 
            event, _ = waiting_for_player_window.read()
            message = self.Oasis.receive_message_protocol()
            if event == sg.WIN_CLOSED: 
                self.Oasis.disconnect_from_server_protocol()
                response = self.Oasis.receive_message_protocol()
                if response["type"] == DISCONNECT_SUCCESS:
                    break
            elif event == "principal_menu_button": 
                self.Oasis.disconnect_from_server_protocol()
                response = self.Oasis.receive_message_protocol()
                if response["type"] == DISCONNECT_SUCCESS:
                    print("I am here")
                    waiting_for_player_window.close()
                    self.window.un_hide()
            elif message is not None: 
                if message["type"] == JOIN_ROOM_SUCCESS: 
                    self.game_menu()
        waiting_for_player_window.close()        

    def join_room_interface(self):
        join_room_layout = [
            [sg.Text("Introduce your nickname:")],
            [sg.InputText(key="nickname")],
            [sg.Text("Introduce the room code:")],
            [sg.InputText(key="room_code")],
            [sg.Button("Join room", key="join_room_button")]
            [sg.Button("Principal menu", key="principal_menu_button")]
        ]
        join_room_window = sg.Window("Join room", join_room_layout, finalize=True)
        while True:
            event, values = join_room_window.read()
            if event == sg.WIN_CLOSED: 
                break
            elif event == "join_room_button":
                nickname = values["nickname"]
                room_code = values["room_code"]
                if nickname == "" or room_code == "": 
                    sg.popup("Invalid nickname or room code")   
                else: 
                    success = self.Oasis.join_room_protocol(room_code, nickname)
                    if success: 
                        join_room_window.close()
                        self.game_menu()
            elif event == "principal_menu_button": 
                join_room_window.close()
                self.window.un_hide()
        join_room_window.close()

    def main_menu_interface(self):
        self.main_layout = [  
            [sg.Text("Welcome to TelePong")],
            [sg.Button("Create room", key="create_room_button")],
            [sg.Button("Join room", key="join_room_button")],
            [sg.Output(size=(50, 10))],  
        ]

        self.window = sg.Window("TelePong", self.main_layout)
        while True:
            event, _ = self.window.read()
            if event == sg.WIN_CLOSED:
                break
            elif event == "create_room_button":
                self.window.hide()                
                self.create_room_interface()
                break
            elif event == "join_room_button":
                self.window.hide()
                self.join_room_interface()
                break
        self.window.close()

    def game_menu(self):
        self.create_listener_thread()
        game_menu_layout = [
            [sg.Text("Game menu")],
            [sg.Button("Ready", key="ready_button")],
            [sg.Button("Not ready", key="not_ready_button")],
            [sg.Button("Exit", key="exit_button")],
        ]
        self.window.Layout("PONG MENU", game_menu_layout, finalize=True)
        while True: 
            event, _ = self.window.read()
            if event == sg.WIN_CLOSED: 
                self.Oasis.disconnect_from_server_protocol()
                break
            elif event == "ready_button": 
                self.Oasis.ready_protocol()
            elif event == "not_ready_button": 
                self.Oasis.not_ready_protocol()
            elif event == "exit_button": 
                self.Oasis.disconnect_from_server_protocol()
                self.window.Layout(self.main_layout)
        self.window.close()
        
    def create_listener_thread(self):
        listener_thread = threading.Thread(target=self.proccess_messages).start()
        listener_thread.start()

    def proccess_messages(self):
        while True:
            messages = self.Oasis.receive_message_protocol()
            if messages["type"] == DISCONNECT_SUCCESS:
                print("Disconnected from server")
                break
            elif messages["type"] == PLAYER2_DISCONNECTED:
                print("Player 2 disconnected")
                print("Waiting for a new player")
            elif messages["type"] == PLAYER1_DISCONNECTED:
                print("Player 1 disconnected")
                print("Waiting for a new player")
            elif messages["type"] == READY_SUCCESS:
                print("Ready success")
                print(messages["payload"])
            elif messages["type"] == NOT_READY_SUCCESS:
                print("Not ready success")
                print(messages["payload"])
            elif messages["type"] == START_GAME:
                # TODO Logic to start the pong Game!
                print("Game started!")