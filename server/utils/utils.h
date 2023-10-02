#include "../necessary_structs/necessary_structs.h"

int checkFunctions(int function, char* message);
char* generate_random_string(int length);
int add_player_to_room(room* room, int client_socket);
int add_player2_to_room(room* room, int client_socket);
room* find_or_create_room(room* ROOMS[MAX_ROOMS]);
room* find_room_by_id(room* ROOMS[MAX_ROOMS], char* room_id);
room* find_room_by_client_socket(room* ROOMS[MAX_ROOMS], int client_socket);