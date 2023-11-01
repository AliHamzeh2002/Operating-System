#ifndef SUPPLIER_H

#define SUPPLIER_H

#include "user.h"
#include "consts.h"
#include <assert.h>
#include <stdbool.h>

typedef struct{
    bool is_valid;
    Ingredient ingredient;
    int connection_fd;
} IngredientRequest;

static struct{
    IngredientRequest ingredient_request;
} supplier_data;

void msg_handler(char* msg, UserData* user_data, int fd);
void cmd_handler(char* cmd, UserData* user_data);
void answer_request(UserData* user_data);
void init_supplier_data();

#endif