#include "../necessary_structs/necessary_structs.h"
#include "../necessary_structs/necessary_defines.h"
#include <time.h>

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
    if (random_string == NULL) {
        return NULL; // Manejo de error si la asignación de memoria falla
    }
    
    srand(time(NULL)); // Semilla basada en el tiempo actual
    for (int i = 0; i < length; i++) {
        int index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }
    random_string[length] = '\0'; // Agrega el carácter nulo al final

    return random_string;
}
int add_player_to_room(room* room, int client_socket){
    if(room->player1 == NULL){
        room->player1 = malloc(sizeof(player));
        room->player1->client_socket = client_socket;
        printf("Room code: %s\n", room->code);
        return 1;
    }  
    return 0;
}

int add_player2_to_room(room* room, int client_socket){
    if(room->player2 == NULL){
        room->player2 = malloc(sizeof(player));
        room->player2->client_socket = client_socket;
        room->is_full = 1;
        return 1;
    }  
    return 0;
}

room* find_or_create_room(room* ROOMS[MAX_ROOMS]){
    for(int i = 0; i < MAX_ROOMS; i++){
        printf("Position: %d\n", i);
        if(ROOMS[i] == NULL){
            room* room = malloc(sizeof(room));
            char* code_room = generate_random_string(6);
            strcpy(room->code, code_room);
            room->player1 = NULL;
            room->player2 = NULL;
            room->is_full = 0;
            printf("Position find_or_create: %d\n", i);
            ROOMS[i] = room;
            return room;
        }
    }
    return NULL;
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
                printf("Player1: %d\n", ROOMS[i]->player1->client_socket);
                if(ROOMS[i]->player1->client_socket == client_socket){  
                    printf("Player1: client_socket: %d ", client_socket);
                    printf("Player1: Position find_by_client_socket: %d ", i);
                    printf("Player1: code: %s\n", ROOMS[i]->code);
                    return ROOMS[i];
                }
            }
            if(ROOMS[i]->player2 != NULL){
                if(ROOMS[i]->player2->client_socket == client_socket){ 
                    printf("Player2: Position find_by_client_socket: %d\n", i);
                    return ROOMS[i];
                }
            }
        }
    }
    return NULL;
}

void remove_room(room* ROOMS[MAX_ROOMS], room* room) {
    // Liberar memoria de la sala
    free(room);
    // Establecer la entrada en el arreglo de salas como NULL
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (ROOMS[i] == room) {
            ROOMS[i] = NULL;
            break;  // Una vez encontrada y eliminada la sala, salir del bucle
        }
    }
}