#ifndef PARSER_JSON_H_
#define PARSER_JSON_H_

#include "../necessary_structs/necessary_structs.h"

char* create_json_message(protocol_message* message);
struct KeyValue* parse_message(char* buffer);

#endif