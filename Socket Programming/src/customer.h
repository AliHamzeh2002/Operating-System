#ifndef CUSTOMER_H

#define CUSTOMER_H

#include "user.h"
#include "consts.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>

static struct {
    //Menu* menu;

} customer_data;

void msg_handler(char* msg, UserData* user_data, int fd);
void cmd_handler(char* cmd, UserData* user_data);
void handle_show_restaurants_cmd(UserData* user_data);
void handle_show_menu_cmd(UserData* user_data);
void handle_order_food_cmd(UserData* user_data);
void handle_post_restaurants_msg(MsgData msg_data, UserData* user_data);
void handle_open_restaurant_msg(MsgData msg_data, UserData* user_data);
void handle_close_restaurant_msg(MsgData msg_data, UserData* user_data);

#endif


