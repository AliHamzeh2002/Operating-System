#include "customer.h"

void init_customer_data(){

}

void handle_post_restaurants_msg(MsgData msg_data, UserData* user_data){
    char* name = find_argument(msg_data, "name");
    char* port = find_argument(msg_data, "port");
    char* out_format = "%s %s\n";
    size_t out_size = strlen(out_format) + strlen(name) + strlen(port) - 3;
    char* out_msg = (char*)malloc(out_size * sizeof(char));
    assert(out_msg != NULL);
    sprintf(out_msg, out_format, name, port);
    write(1, out_msg, strlen(out_msg));
    write_log("restaurant info received.\n", user_data);
}

void handle_open_restaurant_msg(MsgData msg_data, UserData* user_data){
    char* username = find_argument(msg_data, "username");
    char* out_format = "Restaurant %s opened.\n";
    size_t out_size = strlen(out_format) + strlen(username) - 1;
    char* out_msg = (char*)malloc(out_size * sizeof(char));
    assert(out_msg != NULL);
    sprintf(out_msg, out_format, username);
    write(1, out_msg, strlen(out_msg));
    write_log("a restaurant opened.\n", user_data);
}

void handle_close_restaurant_msg(MsgData msg_data, UserData* user_data){
    char* username = find_argument(msg_data, "username");
    char* out_format = "Restaurant %s closed.\n";
    size_t out_size = strlen(out_format) + strlen(username) - 1;
    char* out_msg = (char*)malloc(out_size * sizeof(char));
    assert(out_msg != NULL);
    sprintf(out_msg, out_format, username);
    write(1, out_msg, strlen(out_msg));
    write_log("a restaurant closed.\n", user_data);
}

void set_timeout(int sig){
    customer_data.is_request_timeout = true;
}

void handle_order_food_cmd(UserData* user_data){
    write(1, ASK_PORT_RESTAURANT, strlen(ASK_PORT_RESTAURANT));
    char port_str[PORT_LENGTH];
    read(1, port_str, PORT_LENGTH);
    write(1, ASK_FOOD_NAME, strlen(ASK_FOOD_NAME));
    char food_name[MAX_NAME_SIZE];
    const int name_size = read(1, food_name, sizeof(food_name));
    food_name[name_size - 1] = '\0';
    int restaurant_port = atoi(port_str);
    const char* msg_format =
            "order-food\n"                // title
            "food-name %s\n"
            "username %s\n"
            "port %d\n";
    size_t msg_size = strlen(msg_format) + name_size + strlen(user_data->username) + MAX_INDEX_LENGTH - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, food_name, user_data->username, user_data->tcp_port);
    //write(1, msg, strlen(msg));
    write(1, WAITING_RESPONSE_MSG, strlen(WAITING_RESPONSE_MSG));
    customer_data.is_request_timeout = false;
    signal(SIGALRM, set_timeout);
    siginterrupt(SIGALRM, 1);
    alarm(REQUEST_MAX_WAIT_SENDER);
    const int fd = connect_tcp_client(restaurant_port);
    if (fd < 0){
        write(1, CONNECTION_ERROR, strlen(CONNECTION_ERROR));
        write_log(CONNECTION_ERROR, user_data);
        alarm(0);
        return;
    }
    send_tcp_msg(fd, msg, MAX_SEND_TRIES);
    char* response = receive_tcp(fd);
    alarm(0);
    if (customer_data.is_request_timeout){
        write(1, TIMEOUT_MSG, strlen(TIMEOUT_MSG));
        write_log("request timeout\n", user_data);
        close(fd);
        return;
    }
    else if (strcmp(response, REJECT_RES) == 0){
        write(1, REQUEST_DENIED, strlen(REQUEST_DENIED));
        write_log(REQUEST_DENIED, user_data);

    }
    else if(strcmp(response, ACCEPT_RES) == 0){
        write(1, REQUEST_ACCEPTED, strlen(REQUEST_ACCEPTED));
        write_log(REQUEST_ACCEPTED, user_data);
    }
    
    close(fd);
    
}


void msg_handler(char* msg, UserData* user_data, int fd){
    MsgData msg_data = parse_msg(msg);
    const char* msg_title = msg_data.title;

    if (strcmp(msg_title, "new") == 0){
        handle_new_user_msg(msg_data, user_data);
    }

    else if (strcmp(msg_title, "post-restaurant") == 0){
        handle_post_restaurants_msg(msg_data, user_data);
    }

    else if(strcmp(msg_title, "open-restaurant") == 0){
        handle_open_restaurant_msg(msg_data, user_data);
    }

    else if(strcmp(msg_title, "close-restaurant") == 0){
        handle_close_restaurant_msg(msg_data, user_data);
    }

}

void handle_show_restaurants_cmd(UserData* user_data){
    const char* msg_format =
            "get-restaurants\n"                // title
            "port %d\n";                      // port
    size_t msg_size = strlen(msg_format) + PORT_LENGTH - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, user_data->tcp_port, user_data->username);
    broadcast_msg(user_data->broadcast_port, msg);
    write(1, USERNAME_PORT, strlen(USERNAME_PORT));
    write_log("show restaurant request sent\n", user_data);

}

void handle_show_menu_cmd(UserData* user_data){
    for (int i = 0; i < user_data->menu->num_of_foods; i++){
        const char* cur_food_name = user_data->menu->foods[i]->name;
        char* msg_format = "%d- %s\n";
        char* msg = (char*)malloc(strlen(msg_format) + strlen(cur_food_name) + 1) + 1;
        sprintf(msg, msg_format, i + 1, cur_food_name);
        write(1, msg, strlen(msg));
    }
    write_log("show menu cmd\n", user_data);
}

void cmd_handler(char* cmd, UserData* user_data){
    char** cmd_args = parse_cmd(cmd);
    char* cmd_name = cmd_args[0];
    if (strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "restaurants") == 0){
        handle_show_restaurants_cmd(user_data);
    }

    else if (strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "menu") == 0){
        handle_show_menu_cmd(user_data);        
    }

    else if (strcmp(cmd_name, "order") == 0 && strcmp(cmd_args[1], "food") == 0){
        handle_order_food_cmd(user_data);        
    }
}

int main(int argc, char *argv[]){
    UserData user_data = init_user(atoi(argv[1]));
    init_customer_data();
    handle_connections(&user_data, msg_handler, cmd_handler);
}