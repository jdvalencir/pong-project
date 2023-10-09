import threading
import PySimpleGUI as sg
from oasis_protocol_codes import *
from pong import pong


class TelePongGUI():
    def __init__(self, Oasis): 
        self.Oasis = Oasis
        sg.theme("DarkAmber")

        layout = [
            [sg.Text('Bienvenido a tu juego en línea')],
            [sg.Button('Crear Sala', key='create_room_button'), 
             sg.Button('Unirse a Sala', key='join_room_button')],
            [sg.Button('Listo', key='Ready_button', disabled=True),
             sg.Button('No listo', key='Not_ready_button', disabled=True),
             sg.Button('Salir de la sala', key='exit_button', disabled=True),],
            [sg.Output(size=(50, 10))],  
        ]

        self.window = sg.Window('TelePong', layout)

        while True: 
            event, values = self.window.read()
            if event == sg.WIN_CLOSED:
                if self.Oasis.client_socket is not None:
                    self.Oasis.disconnect_from_server_protocol()
                break
            elif event == 'create_room_button':
                self.create_room()
                self.window["create_room_button"].update(disabled=True)
                self.window["join_room_button"].update(disabled=True)
                self.window["Ready_button"].update(disabled=True)
                self.window["exit_button"].update(disabled=False)
                self.window["Not_ready_button"].update(disabled=True)
            elif event == 'join_room_button':
                self.join_room()
                self.window["create_room_button"].update(disabled=True)
                self.window["join_room_button"].update(disabled=True)
                self.window["Ready_button"].update(disabled=False)
                self.window["exit_button"].update(disabled=False)
                self.window["Not_ready_button"].update(disabled=True)
            elif event == 'Ready_button':
                self.Oasis.ready_protocol()
                self.window["Ready_button"].update(disabled=True)
                self.window["Not_ready_button"].update(disabled=False)
            elif event == "Not_ready_button": 
                self.Oasis.not_ready_protocol()
                self.window["Ready_button"].update(disabled=False)
                self.window["Not_ready_button"].update(disabled=True)
            elif event == 'exit_button':
                self.Oasis.disconnect_from_server_protocol()
                self.window["create_room_button"].update(disabled=False)
                self.window["join_room_button"].update(disabled=False)
                self.window["Ready_button"].update(disabled=True)
                self.window["exit_button"].update(disabled=True)
                self.window["Not_ready_button"].update(disabled=True)

        self.window.close()

    def create_room(self):
        nickname = sg.popup_get_text('Ingresa tu nickname')
        self.room_code, success = self.Oasis.create_room_protocol(nickname)
        if success:
            print(f"Room code: {self.room_code}")
            # Here create the thread to listen all the messages from the server
            proccess_messages_thread = threading.Thread(target=self.proccess_messages)
            proccess_messages_thread.start()

    def join_room(self):
        room_code = sg.popup_get_text('Ingresa el código de la sala')
        nickname = sg.popup_get_text('Ingresa tu nickname')
        success = self.Oasis.join_room_protocol(room_code, nickname)
        if success:
            # Here create the thread to listen all the messages from the server
            proccess_messages_thread = threading.Thread(target=self.proccess_messages)
            proccess_messages_thread.start()

    def update_ready_buttons(self, disabled_ready=True, disabled_not_ready=True): 
        self.window["Ready_button"].update(disabled=disabled_ready)
        self.window["Not_ready_button"].update(disabled=disabled_not_ready)
