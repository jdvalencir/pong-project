#include "../necessary_structs/necessary_structs.h"
#include "../necessary_structs/necessary_defines.h"
#include "../parser_json/parser_json.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800 
#define HEIGHT 400        
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 80
#define BALL_SIZE 15
#define BALL_SPEED 5
#define PADDLE_SPEED 10

int checkFunctions(int function, char* message){
    if(function < 0){
        perror(message);
        exit(1);
    }
    return 1;
}

char* generate_random_string(int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* random_string = malloc(length + 1);
    if (random_string == NULL) return NULL; // Manejo de error si la asignación de memoria falla
    srand(time(NULL)); // Semilla basada en el tiempo actual
    for (int i = 0; i < length; i++) {
        int index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }
    random_string[length] = '\0'; // Agrega el carácter nulo al final
    return random_string;
}

room* find_or_create_room(room* ROOMS[MAX_ROOMS]){
    for(int i = 0; i < MAX_ROOMS; i++){
        if(ROOMS[i] == NULL){
            ROOMS[i] = malloc(sizeof(room));
            char* code_room = generate_random_string(6);
            if (code_room == NULL) {
                free(ROOMS[i]); // Liberar la memoria de room
                return NULL;
            }
            strcpy(ROOMS[i]->code, code_room);
            ROOMS[i]->player1 = NULL;
            ROOMS[i]->player2 = NULL;
            ROOMS[i]->is_full = 0;
            ROOMS[i]->player1_score = 0;
            ROOMS[i]->player2_score = 0;
            printf("Position find_or_create: %d\n", i);
            free(code_room);
            // Liberar la memoria de code_room después de haberla copiado
            return ROOMS[i];
        }
    }
    return NULL;
}

int add_player_to_room(room* room, int client_socket, char* nickname){
    if(room->player1 == NULL){
        room->player1 = malloc(sizeof(player));
        if (room->player1 == NULL) {
            return 0; 
        }
        room->player1->client_socket = client_socket;
        room->player1->nickname = nickname;
        room->player1->ready = 0;
        if (room->player1->nickname == NULL) {
            free(room->player1); 
            return 0; // Indicar fallo.
        }
        return 1;
    }  
    return 0; // Indicar fallo si la sala ya tiene un jugador.
}

int add_player2_to_room(room* room, int client_socket, char* nickname){
    if(room->player2 == NULL){
        room->player2 = malloc(sizeof(player));
        if (room->player2 == NULL) {
            return 0; 
        }
        room->player2->client_socket = client_socket;
        room->player2->nickname = nickname;
        room->player2->ready = 0;
        if (room->player2->nickname == NULL) {
            free(room->player2); 
            return 0; // Indicar fallo.
        }
        return 1;
    }  
    return 0; // Indicar fallo si la sala ya tiene un jugador.
}

room* find_room_by_id(room* ROOMS[MAX_ROOMS], char* room_id){
    for(int i = 0; i < MAX_ROOMS; i++){
        if(ROOMS[i] != NULL) if(strcmp(ROOMS[i]->code, room_id) == 0){  
            printf("Position find_by_id: %d\n", i);
            return ROOMS[i]; 
            }
    }
    return NULL;
}

room* find_room_by_client_socket(room* ROOMS[MAX_ROOMS], int client_socket){
    for(int i = 0; i < MAX_ROOMS; i++){
        if(ROOMS[i] != NULL){
            printf("Find Room by client: %s\n", ROOMS[i]->code);
            if(ROOMS[i]->player1 != NULL){
                if(ROOMS[i]->player1->client_socket == client_socket) return ROOMS[i];
            }
            if(ROOMS[i]->player2 != NULL){
                if(ROOMS[i]->player2->client_socket == client_socket){ 
                    return ROOMS[i];
                }
            }
        }
    }
    return NULL;
}

int remove_room(room* ROOMS[MAX_ROOMS], room* room) {
    // Establecer la entrada en el arreglo de salas como NULL
    printf("Remove room: %s\n", room->code);
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (ROOMS[i] == room) {
            ROOMS[i] = NULL;
            printf("Position remove: %d\n", i);
            return 1;
        }
    }
    return 0;
}

void write_log(char* message) {
    // Abre el archivo de registro en modo de escritura
    FILE* log_file = fopen("log.txt", "a");  // "a" para agregar (append) al archivo existente
    if (log_file == NULL) {
        perror("Error al abrir el archivo de registro");
        return;
    }
    // Obtiene la hora actual para el registro
    time_t current_time;
    char* time_string = NULL;
    time(&current_time);
    time_string = ctime(&current_time);
    if (time_string == NULL) {
        perror("Error al obtener la hora actual");
        fclose(log_file);
        return;
    }
    time_string[strlen(time_string) - 1] = '\0';  // Elimina el carácter de nueva línea al final
    // Escribe el mensaje de registro en el archivo
    fprintf(log_file, "[%s] %s\n", time_string, message);
    fclose(log_file);

}

protocol_message* create_protocol_message(char* buffer, int client_socket) {
    struct KeyValue *result = parse_message(buffer);
    protocol_message* message = malloc(sizeof(protocol_message));
    if (message == NULL) return NULL;
    message->client_socket = client_socket;
    message->type = NULL;
    message->room_code = NULL;
    message->nickname = NULL;
    message->payload = NULL; 
    message->pos_x_ball = 0;
    message->score_player1 = -1; 
    message->score_player2 = -1;
    for (int i = 0; result[i].key != NULL && result[i].value != NULL; i++) {
        if (strcmp(result[i].key, "type") == 0) {
            message->type = strdup(result[i].value);
        } else if (strcmp(result[i].key, "room_code") == 0) {
            message->room_code = strdup(result[i].value);
        } else if (strcmp(result[i].key, "nickname") == 0) {
            message->nickname = strdup(result[i].value);
        } else if (strcmp(result[i].key, "ball_x") == 0) {
            message->pos_x_ball = atoi(result[i].value);
        } else if (strcmp(result[i].key, "payload") == 0) {
            message->payload = strdup(result[i].value);
        } else if (strcmp(result[i].key, "score_player") == 0) {
            message->score_player1 = atoi(result[i].value);
        }
        free(result[i].key); 
        free(result[i].value); 
    }
    free(result); // Libera la memoria del arreglo de KeyValue
    return message;
}

void free_protocol_message(protocol_message* message) {
    if(message->room_code != NULL) free(message->room_code);
    if (message->payload != NULL) free(message->payload);
    free(message->type);
    free(message);

}

int free_room(room* room) {
    free(room->player1);
    free(room->player2);
    free(room->code); 
    free(room);
    return 1;   
}