#include "../necessary_structs/necessary_structs.h"

int checkFunctions(int function, char* message);
char* generate_random_string(int length);
int add_player_to_room(room* room, int client_socket, char* nickname);
int add_player2_to_room(room* room, int client_socket, char* nickname);
room* find_or_create_room(room* ROOMS[MAX_ROOMS]);
room* find_room_by_id(room* ROOMS[MAX_ROOMS], char* room_id);
room* find_room_by_client_socket(room* ROOMS[MAX_ROOMS], int client_socket);
int remove_room(room* ROOMS[MAX_ROOMS], room* room);
void write_log(const char* message);
protocol_message* create_protocol_message(char* buffer, int client_socket);
void free_protocol_message(protocol_message* message);
void free_room(room* room);