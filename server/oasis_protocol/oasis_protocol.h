#ifndef OASIS_PROTOCOL_H_
#define OASIS_PROTOCOL_H_

#include "../necessary_structs/necessary_structs.h"
// Define comandos del protocolo Oasis
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

#define NOT_READY_SUCCESS "NOT_READY_SUCCESS"

#define PLAYER1_DISCONNECTED "PLAYER1_DISCONNECTED"
#define PLAYER2_DISCONNECTED "PLAYER2_DISCONNECTED"


const char* process_message(const protocol_message* message);
int send_message(int socket, const char* message);
int send_response_message(int client_socket, char* MESSAGE_TYPE, char* payload);
int receive_message(int socket, char* buffer);

#endif