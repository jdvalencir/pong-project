import threading
import time
import tkinter as tk
from tkinter import  ttk
from oasis_protocol_codes import *
from queue import Empty, Queue

class PrincipalMenu(tk.Tk):
    def __init__(self, oasis_protocol):
        super().__init__()
        self.oasis_protocol = oasis_protocol
        self.title("TelePong")

        self.configure(bg="black")
        self.geometry("800x600")
        self.resizable(False, False)
       # Crear un contenedor para el título
        title_frame = tk.Frame(self, bg="lightgray")
        title_frame.pack()
        # Cargar la imagen de "PONG" (reemplaza "nombre_de_tu_imagen.png" con la ruta de tu imagen)
        self.title_image = tk.PhotoImage(file="./assets/title.png")
        title_label = tk.Label(title_frame, image=self.title_image, borderwidth=0)
        title_label.pack()
        style = ttk.Style()
        style.configure('Custom.TButton', foreground='black', background='white', font=('Helvetica', 12))
        button_frame = tk.Frame(self, bg="black")
        button_frame.pack(pady=20)  # Espacio vertical entre el título y los botones

        create_room_button = ttk.Button(button_frame, text="Create Room", style='Custom.TButton', command=self.create_room)
        join_room_button = ttk.Button(button_frame, text="Join Room", style='Custom.TButton', command=self.join_room)

        # Alinear los botones en el contenedor
        create_room_button.grid(row=0, column=0, pady=10)
        join_room_button.grid(row=1, column=0, pady=10)

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
        self.title("TelePong - Create room")
        self.geometry("800x600")
        self.configure(bg="black")
        self.resizable(False, False)
        style = ttk.Style()
        style.configure('Custom.TEntry', font=('Helvetica', 12), padding=5)
        style.configure('Custom.TButton', foreground='black', background='white', font=('Helvetica', 12))
        # Crear un contenedor principal para centrar los elementos
        main_container = tk.Frame(self, bg="black")
        main_container.place(relx=0.5, rely=0.5, anchor=tk.CENTER)
        # Crear un título para la entrada
        nickname_label = tk.Label(main_container, text="Nickname:", bg="black", fg="white", font=('Helvetica', 12))
        nickname_label.pack(pady=10)  # Espaciado entre el título y la entrada
        self.nickname_entry = ttk.Entry(main_container, style='Custom.TEntry', width=30)
        self.nickname_entry.pack(pady=10)  # Espaciado entre la entrada y el botón "Create room"
        create_room_button = ttk.Button(main_container, text="Create room", style='Custom.TButton', command=self.create_room)
        create_room_button.pack(pady=10)  # Espaciado entre los botones "Create room" y "Back"
        back_button = ttk.Button(main_container, text="Back", style='Custom.TButton', command=self.go_back)
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
        self.title("TelePong")
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.geometry("800x600")
        self.resizable(False, False)
        self.configure(bg="black")
        style = ttk.Style()

        style.configure('Custom.TLabel', font=('Helvetica', 16), padding=5, foreground='white', background='black')
        style.configure('Custom.TButton', foreground='black', background='white', font=('Helvetica', 12))
        main_container = tk.Frame(self, bg="black")
        main_container.place(relx=0.5, rely=0.5, anchor=tk.CENTER)
        self.room_code_label = ttk.Label(main_container, text=f" {room_code}", style='Custom.TLabel')
        self.room_code_label.pack(pady=10)
        self.waiting_label = ttk.Label(main_container, text="Waiting for player...", style='Custom.TLabel')
        self.waiting_label.pack(pady=20)
        back_button = ttk.Button(main_container, text="Back", style='Custom.TButton', command=self.go_back)
        back_button.pack(pady=10)

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
        self.title("TelePong - Join room")
        self.geometry("800x600")
        self.configure(bg="black")

        style = ttk.Style()
        style.configure('Custom.TLabel', font=('Helvetica', 16), padding=5, foreground='white', background='black')
        style.configure('Custom.TEntry', font=('Helvetica', 12), padding=5)
        style.configure('Custom.TButton', foreground='black', background='white', font=('Helvetica', 12))

        main_frame = tk.Frame(self, bg="black")
        main_frame.place(relx=0.5, rely=0.5, anchor=tk.CENTER)
        nickname_label = ttk.Label(main_frame, text="Nickname", style='Custom.TLabel')
        nickname_label.pack()
        self.nickname_entry = ttk.Entry(main_frame, text="Nickname", style='Custom.TEntry', width=30)
        self.nickname_entry.pack()
        room_code_label = ttk.Label(main_frame, text="Room code", style='Custom.TLabel')
        room_code_label.pack()
        self.room_code_entry = ttk.Entry(main_frame, text="Room code", style='Custom.TEntry', width=30)
        self.room_code_entry.pack(pady=10)
        join_room_button = ttk.Button(main_frame, text="Join room", style='Custom.TButton', command=self.join_room)
        join_room_button.pack(pady=20)
        back_button = ttk.Button(main_frame, text="Back", style='Custom.TButton', command=self.go_back)
        back_button.pack(pady=10)

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
        self.geometry("800x600")
        self.configure(bg="black")

        style = ttk.Style()
        style.configure('Custom.TButton', font=('Helvetica', 12), padding=5, foreground='black', background='white')
        style.map('Custom.TButton', foreground=[('active', 'black')], background=[('active', 'grey')])

        main_frame = tk.Frame(self, bg="black")
        main_frame.place(relx=0.5, rely=0.5, anchor=tk.CENTER)
        self.label_instructions = ttk.Label(main_frame, text="You can play with the arrow keys", font=('Helvetica', 16), foreground='white', background='black')
        self.ready_button = ttk.Button(main_frame, text="Ready", style='Custom.TButton', command=self.ready)
        self.not_ready_button = ttk.Button(main_frame, text="Not ready", style='Custom.TButton', command=self.not_ready, state=tk.DISABLED)
        self.exit_button = ttk.Button(main_frame, text="Exit", style='Custom.TButton', command=self.go_back)

        self.label_instructions.pack(pady=10)
        self.ready_button.pack(pady=10)
        self.not_ready_button.pack(pady=10)
        self.exit_button.pack(pady=10)

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
                    print(message)
                    self.is_running = False
                    dx = message["pos_x"]
                    self.queue.put(lambda: self.goto_pong_game(dx))

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
    
    def goto_pong_game(self, dx):
        self.destroy()
        PongGame(self.oasis_protocol, dx)   

class PongGame(tk.Tk):
    def __init__(self, oasis_protocol, dx):
        self.WIDTH, self.HEIGHT = 800, 400
        self.PADDLE_WIDTH, self.PADDLE_HEIGHT = 10, 80
        self.BALL_SIZE = 15
        self.BALL_SPEED = 5
        self.PADDLE_SPEED = 10
        self.dy = 5
        self.dx = int(dx)

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
        self.after_id = self.after(16, self.process_queue)

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

        self.canvas.create_line(self.WIDTH // 2, 0, self.WIDTH // 2, self.HEIGHT, fill="white")
        self.canvas.create_text(self.WIDTH // 4, 50, text="Player 1", font=("Arial", 30), fill="white")
        self.canvas.create_text(self.WIDTH // 4 * 3, 50, text="Player 2", font=("Arial", 30), fill="white")
        self.player1_score_text = self.canvas.create_text(self.WIDTH // 4, 100, text="0", font=("Arial", 30), fill="white")
        self.player2_score_text = self.canvas.create_text(self.WIDTH // 4 * 3, 100, text="0", font=("Arial", 30), fill="white")

        self.x = self.WIDTH // 2
        self.y = self.HEIGHT // 2
        self.radius = self.BALL_SIZE // 2
        self.collision_blocked = False
        self.ball = self.canvas.create_oval(self.x - self.radius, self.y - self.radius, 
                                            self.x + self.radius, self.y + self.radius, fill="white")
        self.score_player1 = 0
        self.score_player2 = 0
        self.bind("<Up>", self.move_paddle_up)
        self.bind("<Down>", self.move_paddle_down)
        self.after_ball_id = self.after(20, lambda: self.update_ball(self.dx, self.dy))
        self.collision_detection = True

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
                elif message["type"] == UPDATE_GAME_SUCCESS:
                    payload = message["payload"]
                    if payload == "UP":
                        self.queue.put(self.update_second_player_up)
                    else:
                        self.queue.put(self.update_second_player_down)
                elif message["type"] == BALL_UPDATE_SUCCESS: 
                    dx = int(message["pos_x"])
                    self.dx = dx
                elif message["type"] == PLAYER_SCORED_SUCCESS:
                    print("=====================")
                    print("Message", message)
                    player = message["payload"]
                    dx = int(message["pos_x"])
                    if player == "Player 1 scored!":
                        score_player2 = message["score_player1"]
                        self.queue.put(lambda: self.update_score_player1(score_player2))
                    elif player == "Player 2 scored!":
                        score_player1 = message["score_player1"]
                        self.queue.put(lambda: self.update_score_player1(score_player1))
                    elif player == "You scored!":
                        score_player1 = int(message["score_player1"])
                        self.queue.put(lambda: self.update_score_player2(score_player1))
                    self.x = self.WIDTH // 2
                    self.y = self.HEIGHT // 2
                    self.dx = dx
                    self.dy = self.dy
                    self.queue.put(self.reset)
                elif message["type"] == PLAYER_WON:
                    self.is_running = False
                    self.queue.put(self.goto_player_menu)
                elif message["type"] == PLAYER_LOST:
                    self.is_running = False
                    self.queue.put(self.goto_player_menu)

    def process_queue(self):
        try:
            callback = self.queue.get(block=False)
            if callback is not None:
                callback()
        except Empty:
            pass
        self.after_id = self.after(20, self.process_queue)

    def move_paddle_up(self, event):
        y1 = self.canvas.coords(self.player1_paddle)[1]
        if y1 > 0:
            self.canvas.move(self.player1_paddle, 0, -self.PADDLE_SPEED)
            self.oasis_protocol.move_paddle_protocol("UP")

    def move_paddle_down(self, event):
        y2 = self.canvas.coords(self.player1_paddle)[3]
        if y2 < self.HEIGHT:
            self.canvas.move(self.player1_paddle, 0, self.PADDLE_SPEED)
            self.oasis_protocol.move_paddle_protocol("DOWN")

    def update_second_player_up(self):
        self.canvas.move(self.player2_paddle, 0, self.PADDLE_SPEED)

    def update_second_player_down(self):
        self.canvas.move(self.player2_paddle, 0, -self.PADDLE_SPEED)

    def update_ball(self, dx, dy): 
        self.canvas.move(self.ball, dx, dy)
        ball = self.canvas.coords(self.ball)
        paddle1_coords = self.canvas.coords(self.player1_paddle)
        paddle2_coords = self.canvas.coords(self.player2_paddle)
        self.check_collisions(dx, dy, ball, paddle1_coords, paddle2_coords) 
        self.after_ball_id = self.after(20, lambda: self.update_ball(self.dx, self.dy))

    def check_collisions(self, dx, dy, ball_coords=None, paddle1_coords=None, paddle2_coords=None):
        x1, y1, x2, y2 = ball_coords
        if y1 + self.radius <= 0 or y2 + self.radius >= self.HEIGHT:
            self.dy = -dy
        elif ball_coords[2]  >= paddle1_coords[0] and ball_coords[0]  <= paddle1_coords[2] : 
            if ball_coords[3] >= paddle1_coords[1] and ball_coords[3] <= paddle1_coords[3]: 
                self.dx = abs(dx)
                self.oasis_protocol.update_ball_controller(self.dx)
        elif ball_coords[2] >= paddle2_coords[0] and ball_coords[0] <= paddle2_coords[2]:
            if ball_coords[3] >= paddle2_coords[1] and ball_coords[3] <= paddle2_coords[3]: 
                self.dx = -abs(dx)
                self.oasis_protocol.update_ball_controller(self.dx)
        elif x1 + self.radius <= 0 or x2 + self.radius >= self.WIDTH:
            if self.collision_detection: 
                self.score_player1 += 1
                self.oasis_protocol.player_scored_protocol(self.score_player1)
                self.collision_detection = False
        elif x1 + self.radius >= self.WIDTH or x2 + self.radius <= 0:
            if self.collision_detection: 
                self.score_player2 += 1
                self.oasis_protocol.player_scored_protocol(self.score_player2)
                self.collision_detection = False
        
    def reset(self):
        self.canvas.coords(self.ball, self.x - self.radius, self.y - self.radius, self.x + self.radius, self.y + self.radius)
        self.canvas.coords(self.player1_paddle, 50, self.HEIGHT // 2 - self.PADDLE_HEIGHT // 2, 50 + self.PADDLE_WIDTH, self.HEIGHT // 2 + self.PADDLE_HEIGHT // 2)
        self.canvas.coords(self.player2_paddle, self.WIDTH - 50 - self.PADDLE_WIDTH, self.HEIGHT // 2 - self.PADDLE_HEIGHT // 2, self.WIDTH - 50, self.HEIGHT // 2 + self.PADDLE_HEIGHT // 2)
        self.collision_detection = True
        time.sleep(0.5)

    def update_score_player1(self, player1_score):
        self.canvas.itemconfig(self.player1_score_text, text=str(player1_score))

    def update_score_player2(self, player2_score):
        self.canvas.itemconfig(self.player2_score_text, text=str(player2_score))
    
    def destroy(self):
        self.is_running = False
        self.after_cancel(self.after_id)
        super().destroy()

    def goto_waiting_for_player_menu(self, payload):
        self.destroy()
        WaitingForPlayerMenu(self.oasis_protocol, payload)

    def goto_player_menu(self):
        self.destroy()
        PlayerMenu(self.oasis_protocol)

    def on_closing(self):
        self.exit()

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