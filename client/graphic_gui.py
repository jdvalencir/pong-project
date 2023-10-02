from tkinter import *
from tkinter import simpledialog
import threading  # Importa la biblioteca de threading
from functools import partial
from oasis_protocol_codes import *


WIDTH = 800
HEIGHT = 600

class TelePongGUI():
    def __init__(self, root, controllers): 
        # Principal interface
        self.root = root
        self.controllers = controllers
        self.root.title("TelePong")
        self.root.protocol("WM_DELETE_WINDOW", self.disconnet_from_server)
        self.canvas = Canvas(self.root, width=WIDTH, height=HEIGHT, bg="black")
        self.canvas.pack()
        create_room_button = Button(self.root, text="Create Room", command=partial(self.create_room_threaded))
        create_room_button.pack()
        join_room_button = Button(self.root, text="Join Room", command=partial(self.join_room_threaded))
        join_room_button.pack()

    def create_room_threaded(self):
        threading.Thread(target=self.create_room_interface).start()

    def join_room_threaded(self):
        threading.Thread(target=self.join_room_interface).start() 

    def create_room_interface_waiting(self, room_code):
        for widget in self.root.winfo_children():
            if widget != self.canvas:
                widget.destroy()
        self.canvas.delete("all")
        self.canvas.create_text(WIDTH/2, HEIGHT/2, text="Room Code: " + room_code, font=("Arial", 20), fill="white", tag="room_code")
        self.canvas.create_text(WIDTH/2, HEIGHT/2 + 50, text="Waiting for other player...", font=("Arial", 20), fill="white", tag="waiting_text")
        create_disconnect_button = Button(self.root, text="Leave room", command=partial(self.disconnect_from_button))
        create_disconnect_button.pack()

    def join_room_dialog(self):
        newWin = Tk()
        newWin.withdraw()
        retVal = simpledialog.askstring("Enter Value","Please enter the code room:",parent=newWin)
        newWin.destroy()
        if retVal is None or retVal.strip() == "":
            retVal = None
        return retVal

    def player2_joined_room_interface(self, payload):
        for widget in self.root.winfo_children():
            if widget != self.canvas:
                widget.destroy()
        self.canvas.delete("room_code")
        self.canvas.delete("waiting_text")
        self.canvas.create_text(WIDTH/2, HEIGHT/2, text="Room code: " + payload, font=("Arial", 20), fill="white")
        self.canvas.create_text(WIDTH/2, HEIGHT/2 + 50, text="Player 2 has joined the room", font=("Arial", 20), fill="white", tag="joined_text")
        create_ready_button = Button(self.root, text="Ready", command=partial(self.join_room_dialog))
        create_ready_button.pack()
        create_disconnect_button = Button(self.root, text="Leave room", command=partial(self.disconnect_from_button))
        create_disconnect_button.pack()
        threading.Thread(target=self.handle_interface_desconnections).start()

    def create_room_interface(self):
        room_code, result = self.controllers.create_room_controller()
        if result:
            self.create_room_interface_waiting(room_code)
            self.wait_for_player2_interface()
        else:
            self.create_room_interface(room_code)

    def join_room_interface(self):
        room_code = self.join_room_dialog()
        if room_code is None:
            self.canvas.create_text(WIDTH/2, HEIGHT/2, text="Invalid room code", font=("Arial", 20), fill="white")
            return
        result = self.controllers.join_room_controller(room_code)
        if result:
            self.player2_joined_room_interface(room_code)
        else:
            pass # TODO
        
    def wait_for_player2_interface(self):
        payload, result = self.controllers.wait_for_player2_controller()
        if result:
            self.player2_joined_room_interface(payload)
            threading.Thread(target=self.handle_interface_desconnections).start()
        else:
            pass

    def disconnet_from_server(self):
        result = self.controllers.disconnect_controller()
        if result:
            self.root.destroy()
        else:
            pass # TODO

    def disconnect_from_button(self):
        result = self.controllers.disconnect_controller()
        if result:
            self.restart_gui()
        else:
            pass
    
    def restart_gui(self):
        self.root.withdraw()
        new_gui = TelePongGUI(Tk(), self.controllers)
        new_gui.root.mainloop()

    def handle_interface_desconnections(self): 
        message_json = self.controllers.handle_disconnect_controller()
        if message_json["type"] == PLAYER1_DISCONNECTED:
            self.canvas.delete("joined_text")
            self.canvas.create_text(WIDTH/2, HEIGHT/2 + 50, text="Waiting for other player...", font=("Arial", 20), fill="white", tag="waiting_text")
            self.wait_for_player2_interface()
        elif message_json["type"] == PLAYER2_DISCONNECTED:
            self.canvas.delete("joined_text")
            self.canvas.create_text(WIDTH/2, HEIGHT/2 + 50, text="Waiting for other player...", font=("Arial", 20), fill="white", tag="waiting_text")
            self.wait_for_player2_interface()
        else:
            pass