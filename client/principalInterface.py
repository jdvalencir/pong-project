import threading
import time
import tkinter as tk
from tkinter import  ttk
from oasis_protocol_codes import *
from queue import Empty, Queue
import random

class PrincipalMenu(tk.Tk):
    def __init__(self, oasis_protocol):
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("TelePong")

        create_room_button = ttk.Button(self, text="Create room", command=self.create_room)
        join_room_button = ttk.Button(self, text="Join room", command=self.join_room)

        create_room_button.pack()
        join_room_button.pack()

    def create_room(self):
        self.destroy()
        CreateRoomMenu(self.oasis_protocol)

    def join_room(self):
        self.destroy()
        JoinRoomMenu(self.oasis_protocol)

class CreateRoomMenu(tk.Tk):
    def __init__(self, oasis_protocol):
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("Create room")

        self.nickname_entry = ttk.Entry(self, text="Nickname", width=30)
        create_room_button = ttk.Button(self, text="Create room", command=self.create_room)
        back_button = ttk.Button(self, text="Back", command=self.go_back)

        self.nickname_entry.pack()
        create_room_button.pack()
        back_button.pack()

    def create_room(self):
        nickname = self.nickname_entry.get()
        if nickname == "":
            nickname = "Player1"
        room_code, success = self.oasis_protocol.create_room_protocol(nickname)
        if success:
            print("Room code: ", room_code)
            self.destroy()
            WaitingForPlayerMenu(self.oasis_protocol, room_code)
        else:
            print("The room could not be created")
            self.go_back()

    def go_back(self):
        self.destroy()
        PrincipalMenu(self.oasis_protocol)

class WaitingForPlayerMenu(tk.Tk):
    def __init__(self, oasis_protocol, room_code):
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("Waiting for player")
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.room_code = room_code
        self.room_code_label = ttk.Label(self, text=f"Room code: {self.room_code}")
        self.waiting_label = ttk.Label(self, text="Waiting for player...")
        back_button = ttk.Button(self, text="Back", command=self.go_back)

        self.room_code_label.pack()
        self.waiting_label.pack()
        back_button.pack()

        self.queue = Queue()
        self.is_running = True
        self.thread = threading.Thread(target=self.check_if_player_joined)
        self.thread.daemon = True
        self.thread.start()
        self.after_id = self.after(100, self.process_queue)

    def check_if_player_joined(self):
        while self.is_running:
            message = self.oasis_protocol.receive_message_protocol()
            print("thread", message)
            if message is not None:
                if message["type"] == JOIN_ROOM_SUCCESS:
                    self.is_running = False  # Establecer la bandera a False para finalizar el hilo
                    self.queue.put(self.open_player_menu)

    def process_queue(self):
        try:
            callback = self.queue.get(block=False)
            print("process_queue", callback)
            if callback is not None:
                callback()
        except Empty:
            pass
        self.after_id = self.after(100, self.process_queue)

    def destroy(self):
        self.is_running = False
        self.after_cancel(self.after_id)
        super().destroy()
    
    def open_player_menu(self):
        self.destroy()
        PlayerMenu(self.oasis_protocol)

    def exit(self):
        self.is_running = False
        self.oasis_protocol.disconnect_from_server_protocol()
        response = self.oasis_protocol.receive_message_protocol()
        print("exit", response)
        if response["type"] == DISCONNECT_SUCCESS:
            self.destroy()
        else:
            print("Error disconnecting from server")
            self.destroy()

    def on_closing(self):
        self.exit()

    def go_back(self):
        self.exit()
        PrincipalMenu(self.oasis_protocol)

class JoinRoomMenu(tk.Tk):
    def __init__(self, oasis_protocol):
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("Join room")

        nickname_label = ttk.Label(self, text="Nickname")  
        self.nickname_entry = ttk.Entry(self, text="Nickname", width=30)
        room_code_label = ttk.Label(self, text="Room code")
        self.room_code_entry = ttk.Entry(self, text="Room code", width=30)
        join_room_button = ttk.Button(self, text="Join room", command=self.join_room)
        back_button = ttk.Button(self, text="Back", command=self.go_back)

        nickname_label.pack()
        self.nickname_entry.pack()
        room_code_label.pack()
        self.room_code_entry.pack()
        join_room_button.pack()
        back_button.pack()

    def join_room(self):
        nickname = self.nickname_entry.get()
        room_code = self.room_code_entry.get()            
        success = self.oasis_protocol.join_room_protocol(room_code, nickname)
        if success:
            self.destroy()
            PlayerMenu(self.oasis_protocol)
        else:
            print("The room could not be joined")
            self.go_back()

    def go_back(self):
        self.destroy()
        PrincipalMenu(self.oasis_protocol)

class PlayerMenu(tk.Tk):
    def __init__(self, oasis_protocol):
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("Player menu")
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.ready_button = ttk.Button(self, text="Ready", command=self.ready)
        self.not_ready_button = ttk.Button(self, text="Not ready", command=self.not_ready, state=tk.DISABLED)
        self.exit_button = ttk.Button(self, text="Exit", command=self.go_back)
        self.ready_button.pack()
        self.not_ready_button.pack()
        self.exit_button.pack()

        self.queue = Queue() 
        self.is_running = True
        self.thread = threading.Thread(target=self.listen_server_messages)
        self.thread.daemon = True
        self.thread.start()
        self.after_id = self.after(100, self.process_queue)

    def ready(self):
        self.ready_button.config(state=tk.DISABLED)
        self.not_ready_button.config(state=tk.NORMAL)
        self.oasis_protocol.ready_protocol()

    def not_ready(self):
        self.ready_button.config(state=tk.NORMAL)
        self.not_ready_button.config(state=tk.DISABLED)
        self.oasis_protocol.not_ready_protocol()

    def exit(self):
        self.is_running = False
        self.oasis_protocol.disconnect_from_server_protocol()
        response = self.oasis_protocol.receive_message_protocol()
        print("exit", response)
        if response["type"] == DISCONNECT_SUCCESS:
            self.destroy()
        else:
            print("Error disconnecting from server")
            self.destroy()

    def on_closing(self):
        self.exit()

    def go_back(self):
        self.exit()
        PrincipalMenu(self.oasis_protocol)

    def listen_server_messages(self):
        while self.is_running:
            message = self.oasis_protocol.receive_message_protocol()
            if message is not None:
                if message["type"] == READY_SUCCESS:
                    print(message["payload"])
                elif message["type"] == NOT_READY_SUCCESS:
                    print(message["payload"])
                elif message["type"] == PLAYER1_DISCONNECTED:
                    payload = message["payload"]
                    self.queue.put(lambda: self.goto_waiting_for_player_menu(payload))
                elif message["type"] == PLAYER2_DISCONNECTED:
                    self.is_running = False
                    payload = message["payload"]
                    self.queue.put(lambda: self.goto_waiting_for_player_menu(payload))
                elif message["type"] == START_GAME:
                    print("Start game")
                    self.is_running = False
                    self.queue.put(self.goto_pong_game)

    def process_queue(self):
        try:
            callback = self.queue.get(block=False)
            print("process_queue", callback)
            if callback is not None:
                callback()
        except Empty:
            pass
        self.after_id = self.after(100, self.process_queue)

    def destroy(self):
        self.is_running = False
        self.after_cancel(self.after_id)
        super().destroy()

    def goto_waiting_for_player_menu(self, payload):
        self.destroy()
        WaitingForPlayerMenu(self.oasis_protocol, payload)   
    
    def goto_pong_game(self):
        self.destroy()
        PongGame(self.oasis_protocol)   

class PongGame(tk.Tk):
    def __init__(self, oasis_protocol):
        self.WIDTH, self.HEIGHT = 800, 400
        self.PADDLE_WIDTH, self.PADDLE_HEIGHT = 10, 80
        self.BALL_SIZE = 15
        self.BALL_SPEED = 5
        self.PADDLE_SPEED = 10
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("Pong Game")
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.canvas = tk.Canvas(self, width=self.WIDTH, height=self.HEIGHT, bg="black")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.player1_score = 0
        self.player2_score = 0

        self.queue = Queue()
        self.is_running = True
        self.thread = threading.Thread(target=self.listen_server_messages)
        self.thread.daemon = True
        self.thread.start()
        self.after_id = self.after(100, self.process_queue)

        self.player1_paddle = self.canvas.create_rectangle(
            50, self.HEIGHT // 2 - self.PADDLE_HEIGHT // 2,
            50 + self.PADDLE_WIDTH, self.HEIGHT // 2 + self.PADDLE_HEIGHT // 2,
            fill="white"
        )

        self.player2_paddle = self.canvas.create_rectangle(
            self.WIDTH - 50 - self.PADDLE_WIDTH, self.HEIGHT // 2 - self.PADDLE_HEIGHT // 2,
            self.WIDTH - 50, self.HEIGHT // 2 + self.PADDLE_HEIGHT // 2,
            fill="white"
        )

        self.ball = self.canvas.create_oval(
            self.WIDTH // 2 - self.BALL_SIZE // 2, self.HEIGHT // 2 - self.BALL_SIZE // 2,
            self.WIDTH // 2 + self.BALL_SIZE // 2, self.HEIGHT // 2 + self.BALL_SIZE // 2,
            fill="white"
        )

        self.ball_dx = random.choice((self.BALL_SPEED, -self.BALL_SPEED))
        self.ball_dy = random.choice((self.BALL_SPEED, -self.BALL_SPEED))
    
    def listen_server_messages(self):
        while self.is_running:
            message = self.oasis_protocol.receive_message_protocol()
            if message is not None:
                if message["type"] == PLAYER1_DISCONNECTED:
                    self.is_running = False
                    payload = message["payload"]
                    self.queue.put(lambda: self.goto_waiting_for_player_menu(payload))
                elif message["type"] == PLAYER2_DISCONNECTED:
                    self.is_running = False
                    payload = message["payload"]
                    self.queue.put(lambda: self.goto_waiting_for_player_menu(payload))

    def process_queue(self):
        try:
            callback = self.queue.get(block=False)
            print("process_queue", callback)
            if callback is not None:
                callback()
        except Empty:
            pass
        self.after_id = self.after(100, self.process_queue)

    def destroy(self):
        self.is_running = False
        self.after_cancel(self.after_id)
        super().destroy()

    def goto_waiting_for_player_menu(self, payload):
        self.destroy()
        WaitingForPlayerMenu(self.oasis_protocol, payload)

    def exit(self):
        self.is_running = False
        self.oasis_protocol.disconnect_from_server_protocol()
        response = self.oasis_protocol.receive_message_protocol()
        print("exit", response)
        if response["type"] == DISCONNECT_SUCCESS:
            self.destroy()
        else:
            print("Error disconnecting from server")
            self.destroy()

    def on_closing(self):
        self.exit()

    def go_back(self):
        self.exit()
        PrincipalMenu(self.oasis_protocol)