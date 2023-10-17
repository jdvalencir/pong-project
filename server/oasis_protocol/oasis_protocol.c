#include "./oasis_protocol.h"
#include "../necessary_structs/necessary_structs.h"
#include "../necessary_structs/necessary_defines.h"
#include "../parser_json/parser_json.h"
#include "../game_controllers/game_controllers.h"
#include "../utils/utils.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int process_message(protocol_message* message){
    if(strcmp(message->type, CREATE_ROOM) == 0) {
        printf("Create room command\n");
        write_log(CREATE_ROOM);
        if(create_room_controller(message->client_socket, message->nickname) == 1) return 1; 
        return 0;
    } else if(strcmp(message->type, JOIN_ROOM) == 0) {
        printf("Join room command\n");
        write_log(JOIN_ROOM);
        if (join_room_controller(message->client_socket, message->nickname, message->room_code) == 1) return 1;
        return 0;
    } else if(strcmp(message->type, READY) == 0) {
        printf("Ready command");
        write_log(READY);
        if (ready_controller(message->client_socket) == 1) return 1 ;
        return 0;
    } else if(strcmp(message->type, NOT_READY) == 0) {
        printf("Not ready command\n");
        write_log(NOT_READY);
        if (not_ready_controller(message->client_socket) == 1) return 1;
        return 0;
    }  else if(strcmp(message->type, DISCONNECT) == 0) {
        printf("Disconnect command\n");
        write_log(DISCONNECT);
        if (disconnect_controller(message->client_socket) == 1) return 0;
        return 0;
    } else if (strcmp(message->type, UPDATE_GAME) == 0){
        printf("Update game state command\n");
        write_log(UPDATE_GAME);
        if(update_game_state_controller(message->client_socket, message->payload)) return 1;
        return 0;
    } else if (strcmp(message->type, BALL_UPDATE) == 0){
        printf("Ball update state command\n");
        write_log(BALL_UPDATE);
        if(update_ball_state_controller(message->client_socket, message->pos_x_ball)) return 1; 
        return 0;
    } else if (strcmp(message->type, PLAYER_SCORED) == 0){
        printf("Player score command\n");
        write_log(PLAYER_SCORED);
        if(player_scored_controller(message->client_socket, message->score_player1)) return 1;
        return 0;
    }
}

int send_message(int socket, const char* message){
    int message_size = strlen(message);
    int bytes_sent = send(socket, message, message_size, 0);
    if(bytes_sent != message_size){
        perror("Error sending message");
        return 0;
    }
}

int receive_message(int socket, char* buffer){
    int bytes_received = recv(socket, buffer, BUFFER_SIZE, 0);
    if(bytes_received < 0){
        perror("Error receiving message");
        return 0;
    }
    return bytes_received;
}

int send_response_start_game(int client_socket, char* TYPE, char* nickname, char* room_code, int pos_x_ball, char* payload){
    protocol_message* message = malloc(sizeof(protocol_message));
    message->client_socket = client_socket;
    message->type = TYPE;
    message->room_code = room_code;
    message->nickname = nickname;
    message->pos_x_ball = pos_x_ball;
    message->payload = payload;
    message->score_player1 = -1;
    message->score_player2 = -1;
    char* json_message = create_json_message(message);
    send_message(client_socket, json_message);
    bzero(json_message, sizeof(json_message));
    free(message);
    free(json_message);
}

int send_update_score(int client_socket, char* TYPE, int score_player1, int score_player2, int pos_x_ball, char* payload){
    protocol_message* message = malloc(sizeof(protocol_message));
    message->client_socket = client_socket;
    message->type = TYPE;
    message->room_code = NULL;
    message->nickname = NULL;
    message->pos_x_ball = pos_x_ball;
    message->score_player1 = score_player1;
    message->score_player2 = score_player2;
    message->payload = payload;
    char* json_message = create_json_message(message);
    send_message(client_socket, json_message);
    bzero(json_message, sizeof(json_message));
    free(message);
    free(json_message);
}

int send_response_advanced(int client_socket, char* TYPE, char* nickname, char* room_code, char* payload){
    protocol_message* message = malloc(sizeof(protocol_message));
    message->client_socket = client_socket;
    message->type = TYPE;
    message->room_code = room_code;
    message->nickname = nickname;
    message->payload = payload;
    message->pos_x_ball = 0;
    message->score_player1 = -1;
    message->score_player2 = -1;
    char* json_message = create_json_message(message);
    send_message(client_socket, json_message);
    bzero(json_message, sizeof(json_message));
    free(message);
    free(json_message);
}

int send_response_message(int client_socket, char* TYPE, char* payload){
    protocol_message* message = malloc(sizeof(protocol_message));
    message->client_socket = client_socket;
    message->type = TYPE;
    message->room_code = NULL;
    message->nickname = NULL;
    message->payload = payload;
    message->pos_x_ball = 0;
    message->score_player1 = -1;
    message->score_player2 = -1;
    char* json_message = create_json_message(message);
    send_message(client_socket, json_message); 
    free(message);
    free(json_message);
}

void bind_socket(int server_socket, SA_IN server_address){
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    checkFunctions((bind(server_socket, (SA*)&server_address, sizeof(server_address))), "Bind Failed!");
}

void* handle_connection(void* client_socket_arg){
    int client_socket = *((int*)client_socket_arg); // Convierte el argumento a un entero
    protocol_message* message = NULL;
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = receive_message(client_socket, buffer);
        if (bytes_received <= 0) break; // Si no se recibieron bytes o se produjo un error, salimos del bucle
        message = create_protocol_message(buffer, client_socket);
        if (message != NULL) { 
            printf("=======================================\n");
            printf("Message type: %s\n", message->type);
            int processed_message = process_message(message);
            if (processed_message == 0) break;
            printf("=======================================\n");
            free_protocol_message(message);
        }
        bzero(buffer, BUFFER_SIZE);
    } 
    if(message->nickname != NULL) free(message->nickname);
    free_protocol_message(message);
    close(client_socket);
    pthread_detach(pthread_self());
    return NULL;
}

void start_server_function() {
    int server_socket, client_socket, address_size; 
    SA_IN server_address, client_address;
    pthread_t thread;
    address_size = sizeof(SA_IN);
    checkFunctions((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create the socket!"); 
    bind_socket(server_socket, server_address); 
    checkFunctions((listen(server_socket, MAX_CURRENT_USERS)), "Listen Failed!");
    while(1){
        printf("Waiting for players!\n");
        checkFunctions((client_socket = accept(server_socket, (SA*)&client_address, (socklen_t*)&address_size)), "Accept failed!");
        printf("Connection accepted!\n");
        int* client_socket_arg = malloc(sizeof(int));
        *client_socket_arg = client_socket;
        pthread_create(&thread, NULL, &handle_connection, (void*)client_socket_arg);
        sleep(1);
        free(client_socket_arg);
    }
}