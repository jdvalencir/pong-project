#include "game_controllers.h"
#include "../necessary_structs/necessary_structs.h"
#include "../necessary_structs/necessary_defines.h"
#include "../parser_json/parser_json.h"
#include "../oasis_protocol/oasis_protocol.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

room* ROOMS[MAX_ROOMS];

// Luego, en tu función principal o en cualquier otro lugar donde necesites inicializar las salas:
// Controller to create a room
int create_room_controller(int client_socket){
    room* room = find_or_create_room(ROOMS);
    printf("Room code: %s\n", room->code);
    if(room != NULL && add_player_to_room(room, client_socket)){
        send_response_message(client_socket, CREATE_ROOM_SUCCESS, room->code);
    } else send_response_message(client_socket, CREATE_ROOM_ERROR, "Failed to create room!");
}

// Controller to join a room
int join_room_controller(int client_socket, char* room_id){
    room* room = find_room_by_id(ROOMS, room_id);
    if(room == NULL){
        send_response_message(client_socket, JOIN_ROOM_ERROR, "Room not found!");
        return 0;
    }
    if(add_player2_to_room(room, client_socket)){
        send_response_message(client_socket, JOIN_ROOM_SUCCESS, "Joined room successfully!");
        send_response_message(room->player1->client_socket, JOIN_ROOM_SUCCESS, room->code);
    } else send_response_message(client_socket, JOIN_ROOM_ERROR, "Failed to join room!");
}

// Controller to set player as ready
int ready_controller(int client_socket){
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room == NULL){
        send_response_message(client_socket, READY_ERROR, "You are not in a room!");
        return 0;
    } else {
        if(room->player1->client_socket == client_socket){ 
            room->player1->ready = 1; 
            send_response_message(room->player2->client_socket, READY_SUCCESS, "Player 1 is ready!");  
            if(room->player1->ready && room->player2->ready){
                printf("Both players are ready!\n");
                send_response_message(room->player1->client_socket, START_GAME, "Both players are ready!");
                send_response_message(room->player2->client_socket, START_GAME, "Both players are ready!");
            }   
        }  
        else if(room->player2->client_socket == client_socket) {
            room->player2->ready = 1;
            printf("Player 2 is ready!\n");
            send_response_message(room->player1->client_socket, READY_SUCCESS, "Player 2 is ready!");
            if(room->player1->ready && room->player2->ready){
                printf("Both players are ready!\n");
                send_response_message(room->player1->client_socket, START_GAME, "Both players are ready!");
                send_response_message(room->player2->client_socket, START_GAME, "Both players are ready!");
            }
        }
    }
}

// Controller to set player as not ready
int not_ready_controller(int client_socket){
    printf("Not ready controller\n");
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room->player1->client_socket == client_socket) { 
        room->player1->ready = 0; 
        send_response_message(room->player2->client_socket, NOT_READY_SUCCESS, "Player 1 is not ready!");    
    }  
    else if(room->player2->client_socket == client_socket){
        room->player2->ready = 0;
        send_response_message(room->player1->client_socket, NOT_READY_SUCCESS, "Player 2 is not ready!");
    }
}

// Controller to disconnect a player
int disconnect_controller(int client_socket){
    printf("Room[0] code: %s\n", &ROOMS[0]->code);
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room == NULL){
        send_response_message(client_socket, DISCONNECT_ERROR, "You are not in a room!");
    } else {
        if(room->player1->client_socket == client_socket && room->player2 == NULL) {
            send_response_message(client_socket, DISCONNECT_SUCCESS, "Player 1 disconnected!");
            printf("Player 1 disconnected!\n");
            printf("Room %s\n", room->code);
            for(int i = 0; i <= MAX_ROOMS; i++){
                printf("ROOMS[%d]: %s\n", i, &ROOMS[i]->code);
                if(ROOMS[i] == room){  
                    printf("Position find_by_id: %d\n", i);
                    ROOMS[i] = NULL;
                    break;
                }
            }
        } else if(room->player1->client_socket == client_socket && room->player2 != NULL){
            send_response_message(client_socket, DISCONNECT_SUCCESS, "Player 1 disconnected!");
            send_response_message(room->player2->client_socket, PLAYER1_DISCONNECTED, "Player 1 disconnected!");
            // Encapsular en una función para no repetir código
            room->player1 = room->player2;
            room->player1->ready = 0;
            room->player2 = NULL;
        } else if(room->player2->client_socket == client_socket) {
            send_response_message(room->player1->client_socket, PLAYER2_DISCONNECTED, "Player 2 disconnected!");
            send_response_message(client_socket, DISCONNECT_SUCCESS, "Player 2 disconnected!");
            room->player1->ready = 0;
            room->player2 = NULL;
        }
    } 
}
