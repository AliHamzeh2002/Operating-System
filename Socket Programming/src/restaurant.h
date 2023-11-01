#ifndef RESTAURANT_H
#define RESTUARNT_H

#include "user.h"
#include "consts.h"
#include <assert.h>
#include <stdbool.h>


typedef enum{
    TIMEOUT,
    ACCEPTED,
    DENIED,
    PENDING
} OrderStatus;

typedef struct{
    OrderStatus status;
    Food* food;
    int port;
    int fd;
    char* username;
} Order;


static struct{
    bool is_open;
    Ingredient* ingredients;
    size_t num_of_ingredients;
    size_t size_of_ingredients;
    Order* orders;
    size_t num_of_orders;
    size_t size_of_orders;

} restaurant_data;


void msg_handler(char* msg, UserData* user_data, int fd);
void cmd_handler(char* cmd, UserData* user_data);
void handle_get_restaurants_msg(MsgData msg_data, UserData* user_data);
void handle_order_food_msg(MsgData msg_data, UserData* user_data, int fd);
void start_restaurant(UserData* user_data);
void close_restaurant(UserData* user_data);
void init_restaurant_data(UserData* user_data);
void init_ingredients(UserData* user_data);
void handle_show_recipes_cmd(UserData* user_data);
void handle_show_suppliers_cmd(UserData* user_data);
void handle_post_supplier_msg(MsgData msg_data, UserData* user_data);
void handle_request_ingredient_cmd(UserData* user_data);
void increase_ingredient(char* ingredient_name, int amount);
void handle_show_ingredients_cmd(UserData* user_data);
void handle_show_requests_cmd(UserData* user_data);
void add_order(Order order);
void handle_show_requests_cmd(UserData* user_data);
void handle_show_sales_cmd(UserData* user_data);
void answer_request(UserData* user_data);
int find_ingredient_index(char* ingredient_name);
Food* find_food_by_name(Menu* menu, char* food_name);               

#endif