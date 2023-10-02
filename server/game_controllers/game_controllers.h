#ifndef GAME_CONTROLLERS_H
#define GAME_CONTROLLERS_H
 
#include "../necessary_structs/necessary_structs.h"

int create_room_controller(int client_socket);
int join_room_controller(int client_socket, char* room_id);
int ready_controller(int client_socket);
int chat_controller(int client_socket);
int start_game_controller(int client_socket);
int game_state_controller(int client_socket);
int end_game_controller(int client_socket);
int disconnect_controller(int client_socket);

#endif