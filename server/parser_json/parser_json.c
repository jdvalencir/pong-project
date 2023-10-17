#include "../necessary_structs/necessary_structs.h"
#include "parser_json.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char* create_json_message(protocol_message* message) {
    char json_str[2048]; // Ajusta el tamaño según tus necesidades
    memset(json_str, 0, sizeof(json_str)); // Inicializa el búfer a ceros
    // Comienza la construcción del mensaje JSON con los campos obligatorios
    snprintf(json_str, sizeof(json_str), "{\"client_socket\":\"%d\",\"type\":\"%s\",\"payload\":\"%s\"",
             message->client_socket, message->type, message->payload);
    // Agrega campos opcionales si están presentes
    if (message->room_code != NULL) {
        snprintf(json_str + strlen(json_str), sizeof(json_str) - strlen(json_str), ",\"room_code\":\"%s\"", message->room_code);
    }
    if (message->nickname != NULL) {
        snprintf(json_str + strlen(json_str), sizeof(json_str) - strlen(json_str), ",\"nickname\":\"%s\"", message->nickname);
    }
    if (message->pos_x_ball != 0) {
        snprintf(json_str + strlen(json_str), sizeof(json_str) - strlen(json_str), ",\"pos_x\":\"%d\"", message->pos_x_ball);
    }
    if (message->score_player1 != -1) {
        snprintf(json_str + strlen(json_str), sizeof(json_str) - strlen(json_str), ",\"score_player1\":\"%d\"", message->score_player1);
    }
    if (message->score_player2 != -1) {
        snprintf(json_str + strlen(json_str), sizeof(json_str) - strlen(json_str), ",\"score_player2\":\"%d\"", message->score_player2);
    }
    // Cierra el objeto JSON
    strcat(json_str, "}");

    char *json_message = malloc(strlen(json_str) + 1);
    if (json_message == NULL) {
        return NULL;
    }
    strcpy(json_message, json_str);
    bzero(json_str, sizeof(json_str));
    return json_message;
}

struct KeyValue* parse_message(char *buffer) {
    const char* delimiters = " ";
    char* token = strtok(buffer, delimiters);
    struct KeyValue *keyValueArray = NULL;
    int count = 0;
    while (token != NULL) {
        if (count % 2 == 0) {
            keyValueArray = realloc(keyValueArray, (count / 2 + 1) * sizeof(struct KeyValue));
            if (token[0] == '\"'){  
                token++; 
            } 
            keyValueArray[count / 2].key = strdup(token);
        } else {
            if(token[strlen(token) - 1] == '\"') token[strlen(token) - 1] = '\0';
            keyValueArray[count / 2].value = strdup(token);
        }
        token = strtok(NULL, delimiters);
        count++;
    }
    count++;
    keyValueArray = realloc(keyValueArray, (count / 2 + 1) * sizeof(struct KeyValue));
    keyValueArray[count / 2].key = NULL;
    keyValueArray[count / 2].value = NULL;
    return keyValueArray;
}