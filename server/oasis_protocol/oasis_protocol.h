#ifndef OASIS_PROTOCOL_H_
#define OASIS_PROTOCOL_H_

#include "../necessary_structs/necessary_structs.h"

#define JOIN_ROOM "JOIN_ROOM"
#define JOIN_ROOM_SUCCESS "JOIN_ROOM_SUCCESS"
#define JOIN_ROOM_ERROR "JOIN_ROOM_ERROR"

#define CREATE_ROOM "CREATE_ROOM"
#define CREATE_ROOM_SUCCESS "CREATE_ROOM_SUCCESS"
#define CREATE_ROOM_ERROR "CREATE_ROOM_ERROR"

#define READY "READY"
#define READY_SUCCESS "READY_SUCCESS"
#define READY_ERROR "READY_ERROR"

#define CHAT "CHAT"
#define CHAT_SUCCESS "CHAT_SUCCESS"
#define CHAT_ERROR "CHAT_ERROR"

#define START_GAME "START_GAME"
#define START_GAME_SUCCESS "START_GAME_SUCCESS"
#define START_GAME_ERROR "START_GAME_ERROR"

#define GAME_STATE "GAME_STATE"
#define GAME_STATE_SUCCESS "GAME_STATE_SUCCESS"
#define GAME_STATE_ERROR "GAME_STATE_ERROR"

#define END_GAME "END_GAME"
#define END_GAME_SUCCESS "END_GAME_SUCCESS"
#define END_GAME_ERROR "END_GAME_ERROR"

#define DISCONNECT "DISCONNECT"
#define DISCONNECT_SUCCESS "DISCONNECT_SUCCESS"
#define DISCONNECT_ERROR "DISCONNECT_ERROR"

#define LEAVE_ROOM "LEAVE_ROOM"
#define LEAVE_ROOM_SUCCESS "LEAVE_ROOM_SUCCESS"
#define LEAVE_ROOM_ERROR "LEAVE_ROOM_ERROR"

#define NOT_READY "NOT_READY"
#define NOT_READY_SUCCESS "NOT_READY_SUCCESS"
#define NOT_READY_ERROR "NOT_READY_ERROR"

#define PLAYER1_DISCONNECTED "PLAYER1_DISCONNECTED"
#define PLAYER2_DISCONNECTED "PLAYER2_DISCONNECTED"

#define UPDATE_GAME "UPDATE_GAME"
#define UPDATE_GAME_SUCCESS "UPDATE_GAME_SUCCESS"
#define UPDATE_GAME_ERROR "UPDATE_GAME_ERROR"

#define BALL_UPDATE "BALL_UPDATE"
#define BALL_UPDATE_SUCCESS "BALL_UPDATE_SUCCESS"
#define BALL_UPDATE_ERROR "BALL_UPDATE_ERROR"

#define PLAYER_SCORED "PLAYER_SCORED"
#define PLAYER_SCORED_SUCCESS "PLAYER_SCORED_SUCCESS"
#define PLAYER_SCORED_ERROR "PLAYER_SCORED_ERROR"

#define PLAYER_WON "PLAYER_WON"
#define PLAYER_LOST "PLAYER_LOST"

int process_message(protocol_message* message);
int send_message(int socket, const char* message);
int receive_message(int socket, char* buffer);
int send_response_message(int client_socket, char* MESSAGE_TYPE, char* payload);
int send_response_advanced(int client_socket, char* TYPE, char* nickname, char* room_code, char* payload);
int send_response_start_game(int client_socket, char* TYPE, char* nickname, char* room_code, int pos_x_ball, char* payload);
int send_update_score(int client_socket, char* TYPE, int score_player1, int score_player2, int pos_x_ball, char* payload);
void bind_socket(int server_socket, SA_IN server_address); 
void* handle_connection(void* client_socket_arg);
void start_server_function();

#endif