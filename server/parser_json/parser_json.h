#ifndef PARSER_JSON_H_
#define PARSER_JSON_H_

#include "../necessary_structs/necessary_structs.h"

const char* create_json_message(const protocol_message* message);
struct KeyValue* parse_json_message(const char* json_string, int* count);

#endif