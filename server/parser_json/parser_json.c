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

struct KeyValue *parse_json_message(const char *jsonString, int *count) {
    const char *ptr = jsonString;
    struct KeyValue *kv = NULL;
    *count = 0;

    // Verifica si el JSON comienza con '{'.
    if (*ptr != '{') {
        printf("Error: El JSON debe comenzar con '{'\n");
        return NULL;
    }
    ptr++;

    while (*ptr && *ptr != '}') {
        // Ignora espacios en blanco y comas.
        while (*ptr && (*ptr == ' ' || *ptr == ',')) ptr++;

        // Encuentra la clave (debe ser una cadena entre comillas).
        if (*ptr == '"') {
            ptr++;
            int keyIndex = 0;
            char keyBuffer[256];

            while (*ptr && *ptr != '"') {
                keyBuffer[keyIndex] = *ptr;
                keyIndex++;
                ptr++;
            }

            keyBuffer[keyIndex] = '\0';

            // Avanza al separador ":".
            while (*ptr && *ptr != ':') ptr++;

            if (*ptr == ':') {
                ptr++;

                // Ignora espacios en blanco.
                while (*ptr && (*ptr == ' ')) ptr++;

                // Encuentra el valor (puede ser una cadena entre comillas o un número).
                char valueBuffer[256];
                int valueIndex = 0;

                if (*ptr == '"') {
                    ptr++;  // Avanza la comilla inicial.
                    while (*ptr && *ptr != '"') {
                        valueBuffer[valueIndex] = *ptr;
                        valueIndex++;
                        ptr++;
                    }
                    if (*ptr == '"') {
                        ptr++;  // Avanza la comilla final.
                    }
                } else {
                    // Si no es una cadena entre comillas, es un número o valor sin comillas.
                    while (*ptr && *ptr != ',' && *ptr != '}') {
                        valueBuffer[valueIndex] = *ptr;
                        valueIndex++;
                        ptr++;
                    }
                }

                valueBuffer[valueIndex] = '\0';

                // Almacena el par clave-valor en la estructura.
                struct KeyValue *new_kv = realloc(kv, (*count + 1) * sizeof(struct KeyValue));

                if (new_kv == NULL) {
                    printf("Error: Falta de memoria al analizar el JSON\n");
                    free_kv_array(kv, *count);
                    return NULL;
                }

                kv = new_kv;
                kv[*count].key = strdup(keyBuffer);
                kv[*count].value = strdup(valueBuffer);
                (*count)++;
            } else {
                printf("Error: Se esperaba un ':' después de la clave\n");
                free_kv_array(kv, *count);
                return NULL;
            }
        } else {
            printf("Error: Se esperaba una clave entre comillas\n");
            free_kv_array(kv, *count);
            return NULL;
        }
    }

    // Verifica si el JSON termina con '}'.
    if (*ptr != '}') {
        printf("Error: El JSON debe terminar con '}'\n");
        free_kv_array(kv, *count);
        return NULL;
    }

    return kv;
}
// Función auxiliar para liberar la memoria asignada a un arreglo de KeyValue.
void free_kv_array(struct KeyValue *kv, int count) {
    for (int i = 0; i < count; i++) {
        free(kv[i].key);
        free(kv[i].value);
    }
    free(kv);
}
