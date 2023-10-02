#include "../necessary_structs/necessary_structs.h"
#include "parser_json.h"
#include <string.h>

const char* create_json_message(const protocol_message* message) {
    char json_str[1076]; 
    snprintf(json_str, sizeof(json_str), "{\"client_socket\":\"%d\",\"type\":\"%s\",\"payload\":\"%s\"}", message->client_socket, message->type, message->payload);
    char *json_message = malloc(strlen(json_str) + 1);
    if (json_message == NULL) {
        return NULL; 
    }
    strcpy(json_message, json_str);
    return json_message;
}

struct KeyValue *parse_json_message(const char *jsonString) {
    char keyBuffer[256];
    char valueBuffer[256];
    const char *ptr = jsonString;
    struct KeyValue *kv = malloc(sizeof(struct KeyValue) * 2); 
    int kvCount = 0;

    while (*ptr && *ptr != '{') ptr++;
    while (*ptr && *ptr != '}') {
        while (*ptr && *ptr != '"') ptr++;
        ptr++;
        int keyIndex = 0;
        while (*ptr && *ptr != '"') {
            keyBuffer[keyIndex] = *ptr;
            keyIndex++;
            ptr++;
        }
        keyBuffer[keyIndex] = '\0';
        while (*ptr && *ptr != ':') ptr++;
        ptr++;
        while (*ptr && (*ptr == ' ' || *ptr == '"')) ptr++;
        int valueIndex = 0;
        while (*ptr && *ptr != '"') {
            valueBuffer[valueIndex] = *ptr;
            valueIndex++;
            ptr++;
        }
        valueBuffer[valueIndex] = '\0';
        while (*ptr && (*ptr == ' ' || *ptr == ',')) ptr++;
        kv[kvCount].key = strdup(keyBuffer);
        kv[kvCount].value = strdup(valueBuffer);
        kvCount++;
    }
    kv[kvCount].key = NULL;
    kv[kvCount].value = NULL;
    return kv;
}
