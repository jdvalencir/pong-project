#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "./oasis_protocol/oasis_protocol.h"
#include "./necessary_structs/necessary_structs.h"
#include "./necessary_structs/necessary_defines.h"
#include "./utils/utils.h"
#include "./parser_json/parser_json.h"

typedef struct sockaddr_in SA_IN; 
typedef struct sockaddr SA;

void bind_socket(int server_socket, SA_IN server_address);
int checkFunctions(int function, char* message);
void* handle_connection(void* client_socket);

int main() {
    int server_socket, client_socket, address_size; 
    SA_IN server_address, client_address;
    pthread_t thread;

    checkFunctions((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create the socket!"); 
    bind_socket(server_socket, server_address); 
    checkFunctions((listen(server_socket, MAX_CURRENT_USERS)), "Listen Failed!"); 
    address_size = sizeof(SA_IN);
    while(1){
        printf("Waiting for connections...\n");
        checkFunctions((client_socket = accept(server_socket, (SA*)&client_address, (socklen_t*)&address_size)), 
        "Accept failed!");
        printf("Connection accepted!\n");
        int* client = malloc(sizeof(int));
        *client = client_socket;
        pthread_create(&thread, NULL, &handle_connection, (void*)client);
        sleep(1);
    }    
    return 0; 
}

void* handle_connection(void* client_socket){
    int socket = *(int*)client_socket;
    char buffer[BUFFER_SIZE];
    send_response_message(socket, "WELCOME", "Welcome to the server!");  
    while (1) {
        int bytes_received = receive_message(socket, buffer);
        if(bytes_received <= 0) close(client_socket);
        if(!bytes_received) printf("Connection lost!\n");

        struct KeyValue* kv = parse_json_message(buffer);
        protocol_message* message = malloc(sizeof(protocol_message));
        message->client_socket = socket;
        message->type = kv[0].value;
        printf("Message type: %s\n", message->type);
        message->payload = kv[1].value;
        char* proccessed_message = process_message(message);
        if(strcmp(proccessed_message, DISCONNECT) == 0) {
            close(socket);
            free(client_socket);
            pthread_detach(pthread_self());
            return NULL;
        }
        bzero(buffer, BUFFER_SIZE);
    }
}

void bind_socket(int server_socket, SA_IN server_address){
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    checkFunctions((bind(server_socket, (SA*)&server_address, sizeof(server_address))), "Bind Failed!");
}