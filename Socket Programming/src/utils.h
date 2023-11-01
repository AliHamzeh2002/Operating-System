#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "cJSON.h"
#include "consts.h"

typedef struct {
    char* name;
    char* value;
} Argument;

typedef struct {
    char* title;
    Argument* arguments;
    int num_of_arguments;
} MsgData;

typedef struct {
    char* name;
    size_t quantity;
} Ingredient;

typedef struct{
    char* name;
    Ingredient* ingredients;
    size_t num_of_ingredients;
} Food;

typedef struct{
    Food** foods;
    size_t num_of_foods;
} Menu;

MsgData parse_msg(char* msg);
char* find_argument(MsgData msg_data, char* argument_name);
char** parse_cmd(char* cmd);

char* json_to_string(const char* json_file_name);
Food* parse_food(cJSON* food_json);
Menu* parse_menu(cJSON* root);
Menu* read_json(const char* json_file_name);


#endif 