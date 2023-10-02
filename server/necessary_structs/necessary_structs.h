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
    char* nick_name;
    int ready;
} player;

typedef struct Game_state {
    int player1Y;
    int player2Y;
    int score_player1;
    int score_player2;
    int ball_x;
    int ball_y;
    int ball_velocity_x;
    int ball_velocity_y;
    int ball_radius;
} game_state;

typedef struct Room{
    char code[LENGTH_CODE_ROOM];
    player* player1;
    player* player2; 
    int is_full;
    game_state* game_state;
    pthread_t room_thread;
    pthread_mutex_t mutex; 
    pthread_cond_t cond;
} room;

typedef struct Protocol_message {
    int client_socket;
    char* type;
    char* payload; 
} protocol_message;

#endif