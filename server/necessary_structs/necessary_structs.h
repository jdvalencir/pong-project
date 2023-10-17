#ifndef NECESSARY_STRUCTS_H_
#define NECESSARY_STRUCTS_H_

#include <pthread.h>
#define LENGTH_CODE_ROOM 6

struct KeyValue {
    char *key;
    char *value;
};

typedef struct Player {
    int client_socket; 
    char* nickname;
    int ready;
} player;

typedef struct Room{
    char code[LENGTH_CODE_ROOM];
    player* player1;
    player* player2; 
    int is_full;
    int player1_score;
    int player2_score;
} room;

typedef struct Protocol_message {
    int client_socket;
    char* type;
    char* room_code;
    char* nickname;
    int pos_x_ball;
    int score_player1;
    int score_player2;
    char* payload; 
} protocol_message;

typedef struct sockaddr_in SA_IN; 
typedef struct sockaddr SA;

#endif