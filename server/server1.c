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
        int* client_socket_arg = malloc(sizeof(int));
        *client_socket_arg = client_socket;

        pthread_create(&thread, NULL, &handle_connection, (void*)client_socket_arg);
        sleep(1);
    }    
    return 0; 
}

void* handle_connection(void* client_socket_arg){
    int client_socket = *((int*)client_socket_arg); // Convierte el argumento a un entero

    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = receive_message(client_socket, buffer);
        printf("Bytes received: %s\n", buffer);
        if (bytes_received <= 0) {
            // Si no se recibieron bytes o se produjo un error, salimos del bucle
            break;
        }

        int count;
        struct KeyValue* kv = parse_json_message(buffer, &count);
        protocol_message* message = malloc(sizeof(protocol_message));
        message->client_socket = client_socket;
        message->type = kv[0].value;
        printf("=======================================\n");
        printf("Message type: %s\n", message->type);
        message->payload = kv[1].value;
        int processed_message = process_message(message);
        printf("Processed message: %d\n", processed_message);
        if(processed_message == 0) break;
        // Haces lo que necesites con processed_message
        printf("=======================================\n");
        bzero(buffer, BUFFER_SIZE);
    }

    // Cerramos el socket y liberamos la memoria
    close(client_socket);
    free(client_socket_arg);
    // Terminamos el hilo
    pthread_exit(NULL);
    return NULL;
}

void bind_socket(int server_socket, SA_IN server_address){
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    checkFunctions((bind(server_socket, (SA*)&server_address, sizeof(server_address))), "Bind Failed!");
}