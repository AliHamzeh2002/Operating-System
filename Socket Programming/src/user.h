#ifndef USER_H
#define USER_H

#include "tcp.h"
#include "udp.h"
#include "consts.h"
#include "utils.h"
#include <stdbool.h>
#include <assert.h>

typedef struct{
    int broadcast_port;
    int tcp_port;
    char* username;
    int udp_fd;
    int tcp_fd;
    int log_fd;
    Menu* menu;
} UserData;

void alarm_handler(int sig);
void show_timeout_msg(int sig);
UserData init_user();
void set_username(UserData* user_data);
void handle_connections(UserData* user_data, void (*msg_handler)(char*, UserData*, int), void (*cmd_handler)(char*, UserData*));
void handle_new_user_msg(MsgData msg_data, UserData* user_data);
void show_timeout_msg(int sig);
void write_log(char* msg, UserData* user_data);





#endif