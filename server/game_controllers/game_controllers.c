#include "../necessary_structs/necessary_structs.h"
#include "../necessary_structs/necessary_defines.h"
#include "../parser_json/parser_json.h"
#include "../oasis_protocol/oasis_protocol.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_controllers.h"

room* ROOMS[MAX_ROOMS] = { NULL };

int create_room_controller(int client_socket, char* nickname){
    room* room = find_or_create_room(ROOMS);
    if(room != NULL && add_player_to_room(room, client_socket, nickname)){
        send_response_advanced(client_socket, CREATE_ROOM_SUCCESS, nickname, room->code, "Room created successfully!");
        write_log(CREATE_ROOM_SUCCESS);
        return 1;
    } else { 
        send_response_message(client_socket, CREATE_ROOM_ERROR, "Failed to create room!"); 
        write_log(CREATE_ROOM_ERROR);
        return 1;
    }
}

int join_room_controller(int client_socket, char* nickname, char* room_id){
    room* room = find_room_by_id(ROOMS, room_id);
    if(room == NULL){
        send_response_message(client_socket, JOIN_ROOM_ERROR, "Room not found!");
        write_log(JOIN_ROOM_ERROR);
        return 1;
    }
    else if(add_player2_to_room(room, client_socket, nickname) == 1){
        send_response_advanced(client_socket, JOIN_ROOM_SUCCESS, nickname, room->code, "Joined room successfully!");
        send_response_advanced(room->player1->client_socket, JOIN_ROOM_SUCCESS, room->player2->nickname, room->code, "Joined room successfully!");
        write_log(JOIN_ROOM_SUCCESS);
        return 1;
    } else {
        send_response_message(client_socket, JOIN_ROOM_ERROR, "Failed to join room!");  
        write_log(JOIN_ROOM_ERROR);
        return 1;
    }
}

int ready_controller(int client_socket){
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room->player1->client_socket == client_socket){ 
        room->player1->ready = 1; 
        send_response_message(room->player2->client_socket, READY_SUCCESS, "Player 1 is ready!");  
        write_log(READY_SUCCESS);
        check_both_player_ready(room);
        return 1; 
    } else if(room->player2->client_socket == client_socket) {
        room->player2->ready = 1;
        send_response_message(room->player1->client_socket, READY_SUCCESS, "Player 2 is ready!");
        write_log(READY_SUCCESS);
        check_both_player_ready(room);
        return 1; 
    } else {
        send_response_message(client_socket, READY_ERROR, "You are not in a room!");
        write_log(READY_ERROR);
        return 0;
    }
}

int check_both_player_ready(room* room){
    if(room->player1->ready == 1 && room->player2->ready == 1){
        int pos_x_ball = (rand() % (5 - (-5) + 1)) + -5;       
        if (pos_x_ball == 0) pos_x_ball = 4;  
        send_response_start_game(room->player1->client_socket, START_GAME, room->player1->nickname, room->code, pos_x_ball, "Both players are ready!");
        send_response_start_game(room->player2->client_socket, START_GAME, room->player2->nickname, room->code, -pos_x_ball, "Both players are ready!");
        write_log(START_GAME);
        return 1;

    }
    return 0;
}

int not_ready_controller(int client_socket){
    printf("Not ready controller\n");
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room->player1->client_socket == client_socket) { 
        room->player1->ready = 0; 
        send_response_message(room->player2->client_socket, NOT_READY_SUCCESS, "Player 1 is not ready!");  
        write_log(NOT_READY_SUCCESS);  
        return 1;
    } else if(room->player2->client_socket == client_socket){
        room->player2->ready = 0;
        send_response_message(room->player1->client_socket, NOT_READY_SUCCESS, "Player 2 is not ready!");
        write_log(NOT_READY_SUCCESS);
        return 1;
    } else { 
        send_response_message(client_socket, NOT_READY_ERROR, "You are not in a room!");
        write_log(NOT_READY_ERROR);
        return 0;
    }
}

int disconnect_controller(int client_socket){
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room->player1->client_socket == client_socket && room->player2 == NULL) {
        if(remove_room(ROOMS, room) ==  1) {
            send_response_message(client_socket, DISCONNECT_SUCCESS, "Player 1 disconnected!");
            write_log(DISCONNECT_SUCCESS);
            printf("Player 1 disconnected\n");
            free(room->player1->nickname);
            free(room->player1);
            free(room);
            return 0;
        } else {
            send_response_message(client_socket, DISCONNECT_ERROR, "Failed to disconnect!");
            write_log(DISCONNECT_ERROR);
            return 0;
        }
    } else if(room->player1->client_socket == client_socket && room->player2 != NULL){
            printf("Player 1 disconnected room player2 null\n");
            send_response_message(client_socket, DISCONNECT_SUCCESS, "Player 1 disconnected!");
            send_response_message(room->player2->client_socket, PLAYER1_DISCONNECTED, "Player 1 disconnected!");
            printf("Player 1 disconnected\n");
            write_log(PLAYER1_DISCONNECTED);
            free(room->player1->nickname);
            free(room->player1);
            room->player1 = room->player2;
            room->player1->ready = 0;
            room->player2 = NULL;
            printf("Player 1 disconnected\n");
            return 0;
    } else if(room->player2->client_socket == client_socket) {
            printf("Player 2 disconnected client socket\n");
            send_response_message(client_socket, DISCONNECT_SUCCESS, "Player 2 disconnected!");
            send_response_message(room->player1->client_socket, PLAYER2_DISCONNECTED, "Player 2 disconnected!");
            printf("Player 2 disconnected\n");
            write_log(PLAYER2_DISCONNECTED);
            room->player1->ready = 0;
            free(room->player2->nickname);
            free(room->player2);
            room->player2 = NULL;
            printf("Player 2 disconnected\n");
            return 0;
    }
}

int update_game_state_controller(int client_socket, char* payload){ 
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room == NULL){
        send_response_message(client_socket, GAME_STATE_ERROR, "You are not in a room!");
    } else {
        if(room->player1->client_socket == client_socket) { 
            if(strcmp(payload, "UP")) send_response_message(room->player2->client_socket, UPDATE_GAME_SUCCESS, "UP");    
            else send_response_message(room->player2->client_socket, UPDATE_GAME_SUCCESS, "DOWN");
            write_log(UPDATE_GAME_SUCCESS);
            return 1;
        }  
        else if(room->player2->client_socket == client_socket){
            if(strcmp(payload, "UP")) send_response_message(room->player1->client_socket, UPDATE_GAME_SUCCESS, "UP");    
            else send_response_message(room->player1->client_socket, UPDATE_GAME_SUCCESS, "DOWN");
            write_log(UPDATE_GAME_SUCCESS);
            return 1;
        }
    }
}

int update_ball_state_controller(int client_socket, int pos_x_ball){
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room == NULL){
        send_response_message(client_socket, BALL_UPDATE_ERROR, "You are not in a room!");
    } else {
        if(room->player1->client_socket == client_socket) { 
            int new_pos_x_ball = -pos_x_ball;
            send_response_start_game(room->player2->client_socket, BALL_UPDATE_SUCCESS, room->player2->nickname, room->code, new_pos_x_ball, "Ball updated!");
            write_log(BALL_UPDATE_SUCCESS);
            return 1;
        }  
        else if(room->player2->client_socket == client_socket){
            int new_pos_x_ball = -pos_x_ball;
            send_response_start_game(room->player1->client_socket, BALL_UPDATE_SUCCESS, room->player1->nickname, room->code, new_pos_x_ball, "Ball updated!");
            write_log(BALL_UPDATE_SUCCESS);
            return 1;
        }
    }
}

int player_scored_controller(int client_socket, int score_player){
    room* room = find_room_by_client_socket(ROOMS, client_socket);
    if(room == NULL){
        send_response_message(client_socket, PLAYER_SCORED_ERROR, "You are not in a room!");
    } else {
        if(room->player1->client_socket == client_socket) { 
            int pos_x_ball = (rand() % (5 - (-5) + 1)) + -5;       
            if (pos_x_ball == 0) pos_x_ball = 4;  
            if (score_player == 5){
                room->player1->ready = 0;
                room->player2->ready = 0;
                send_response_message(room->player1->client_socket, PLAYER_WON, "You won!");
                send_response_message(room->player2->client_socket, PLAYER_LOST, "You lost!");
                return 1;
            }
            send_update_score(room->player1->client_socket, PLAYER_SCORED_SUCCESS, score_player, 0 , pos_x_ball, "Player 2 scored!");
            send_update_score(room->player2->client_socket, PLAYER_SCORED_SUCCESS, score_player, 0, -pos_x_ball, "You scored!");
            write_log(PLAYER_SCORED_SUCCESS);
            return 1;
        }  
        else if(room->player2->client_socket == client_socket){
            int pos_x_ball = (rand() % (5 - (-5) + 1)) + -5;
            if (pos_x_ball == 0) pos_x_ball = 4;
            if (score_player == 5){
                room->player1->ready = 0;
                room->player2->ready = 0;
                send_response_message(room->player2->client_socket, PLAYER_WON, "You won!");
                send_response_message(room->player1->client_socket, PLAYER_LOST, "You lost!");
                return 1;
            }
            send_update_score(room->player2->client_socket, PLAYER_SCORED_SUCCESS, score_player, 0, pos_x_ball, "Player 1 scored!");
            send_update_score(room->player1->client_socket, PLAYER_SCORED_SUCCESS, score_player, 0, -pos_x_ball, "You scored!");
            write_log(PLAYER_SCORED_SUCCESS);
            return 1;
        }
    }
}
