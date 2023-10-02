#include "./oasis_protocol.h"
#include "../necessary_structs/necessary_structs.h"
#include "../necessary_structs/necessary_defines.h"
#include "../parser_json/parser_json.h"
#include "../game_controllers/game_controllers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* process_message(const protocol_message* message){
    if(strcmp(message->type, JOIN_ROOM) == 0) {
        printf("Join room command\n");
        printf("Payload: %s\n", message->payload);
        join_room_controller(message->client_socket, message->payload);
        return JOIN_ROOM;
    } else if(strcmp(message->type, CREATE_ROOM) == 0) {
        printf("Create room command\n");
        create_room_controller(message->client_socket);
        return CREATE_ROOM;
    }  else if(strcmp(message->type, DISCONNECT) == 0) {
        printf("Disconnect command\n");
        disconnect_controller(message->client_socket);
        return DISCONNECT;
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
}

int send_response_message(int client_socket, char* MESSAGE_TYPE, char* payload){
    protocol_message* message = malloc(sizeof(protocol_message));
    message->client_socket = client_socket;
    message->type = MESSAGE_TYPE;
    message->payload = payload;
    char* json_message = create_json_message(message);
    send_message(client_socket, json_message); 
}