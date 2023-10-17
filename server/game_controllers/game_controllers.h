#ifndef GAME_CONTROLLERS_H
#define GAME_CONTROLLERS_H
 
#include "../necessary_structs/necessary_structs.h"

int create_room_controller(int client_socket, char* nickname);
int join_room_controller(int client_socket, char* nickname, char* room_id);
int ready_controller(int client_socket);
int check_both_player_ready(room* room);
int not_ready_controller(int client_socket);
int update_ball_state_controller(int client_socket, int pos_x_ball);
int player_scored_controller(int client_socket, int score_player);
int update_game_state_controller(int client_socket, char* payload);
int disconnect_controller(int client_socket);

#endif