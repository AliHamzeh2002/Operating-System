#include "restaurant.h"

int find_ingredient_index(char* ingredient_name){
    for (int i = 0; i < restaurant_data.num_of_ingredients; i++){
        if (strcmp(restaurant_data.ingredients[i].name, ingredient_name) == 0)
            return i;
    }
    return -1;
}

void init_ingredients(UserData* user_data){
    restaurant_data.ingredients = (Ingredient*)malloc(restaurant_data.size_of_ingredients * sizeof(Ingredient));
    for (int i = 0; i < user_data->menu->num_of_foods; i++){
        for (int j = 0; j < user_data->menu->foods[i]->num_of_ingredients; j++){
            char* cur_ingredient_name = user_data->menu->foods[i]->ingredients[j].name;
            if (find_ingredient_index(user_data->menu->foods[i]->ingredients[j].name) >= 0)
                continue;
            if (restaurant_data.num_of_ingredients == restaurant_data.size_of_ingredients){
                restaurant_data.size_of_ingredients *= 2;
                restaurant_data.ingredients = (Ingredient*)realloc(restaurant_data.ingredients, restaurant_data.size_of_ingredients * sizeof(Ingredient));
            }
            restaurant_data.ingredients[restaurant_data.num_of_ingredients].name = user_data->menu->foods[i]->ingredients[j].name;
            restaurant_data.ingredients[restaurant_data.num_of_ingredients].quantity = 0;
            restaurant_data.num_of_ingredients++;            
        }
    }
}

void init_restaurant_data(UserData* user_data){
    restaurant_data.is_open = false;
    restaurant_data.num_of_ingredients = 0;
    restaurant_data.size_of_ingredients = 2;
    restaurant_data.num_of_orders = 0;
    restaurant_data.size_of_orders = 2;
    restaurant_data.orders = (Order*)malloc(restaurant_data.size_of_orders * sizeof(Order));
    init_ingredients(user_data);
}

void handle_get_restaurants_msg(MsgData msg_data, UserData* user_data){
    int fd = connect_tcp_client(atoi(find_argument(msg_data, "port")));
    const char* msg_format =
            "post-restaurant\n"                // title
            "port %d\n"                        // port
            "name %s\n";                   // name
    const int msg_size = strlen(msg_format) + strlen(user_data->username) + PORT_LENGTH - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, user_data->tcp_port, user_data->username);
    send_tcp_msg(fd, msg, MAX_SEND_TRIES);
    write_log("info sent\n", user_data);
}

void start_restaurant(UserData* user_data){
    if (restaurant_data.is_open)
        return;
    restaurant_data.is_open = true;
    const char* msg_format =
            "open-restaurant\n"                // title
            "username %s\n";       // username
    size_t msg_size = strlen(msg_format) + strlen(user_data->username) - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, user_data->username);
    broadcast_msg(user_data->broadcast_port, msg);
    write_log("restaurant opened\n", user_data);

    
}

void close_restaurant(UserData* user_data){
    if (!restaurant_data.is_open)
        return;
    restaurant_data.is_open = false;
    const char* msg_format =
            "close-restaurant\n"                // title
            "username %s\n";       // username
    size_t msg_size = strlen(msg_format) + strlen(user_data->username) - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, user_data->username);
    broadcast_msg(user_data->broadcast_port, msg);
    write_log("restaurant closed\n", user_data);
}

void handle_post_supplier_msg(MsgData msg_data, UserData* user_data){
    char* name = find_argument(msg_data, "name");
    char* port = find_argument(msg_data, "port");
    char* out_format = "%s %s\n";
    size_t out_size = strlen(out_format) + strlen(name) + strlen(port) - 3;
    char* out_msg = (char*)malloc(out_size * sizeof(char));
    assert(out_msg != NULL);
    sprintf(out_msg, out_format, name, port);
    write(1, out_msg, strlen(out_msg));
    write_log("supplier info received.\n", user_data);
}

void add_order(Order order){
    if (restaurant_data.num_of_orders == restaurant_data.size_of_orders){
        restaurant_data.size_of_orders *= 2;
        restaurant_data.orders = (Order*)realloc(restaurant_data.orders, restaurant_data.size_of_orders * sizeof(Order));
    }
    restaurant_data.orders[restaurant_data.num_of_orders] = order;
    restaurant_data.num_of_orders++;
}

bool has_ingredients(Food* food){
    for (int i = 0; i < food->num_of_ingredients; i++){
        int index = find_ingredient_index(food->ingredients[i].name);
        if (index < 0 || restaurant_data.ingredients[index].quantity < food->ingredients[i].quantity)
            return false;
    }
    return true;
}

void make_food(Food* food){
    for (int i = 0; i < food->num_of_ingredients; i++){
        increase_ingredient(food->ingredients[i].name, food->ingredients[i].quantity);
    }
}

void handle_order_food_msg(MsgData msg_data, UserData* user_data, int fd){
    char* food_name = find_argument(msg_data, "food-name");
    char* username = find_argument(msg_data, "username");
    int port = atoi(find_argument(msg_data, "port"));
    Order new_order;
    new_order.port = port;
    new_order.username = username;
    new_order.fd = fd;
    new_order.food = find_food_by_name(user_data->menu, food_name);
    new_order.status = PENDING;
    if (!has_ingredients(new_order.food)){
        send_tcp_msg(fd, REJECT_RES, MAX_SEND_TRIES);
        write_log("order rejected. not enought ingredients\n", user_data);
        return;
    }
    add_order(new_order);
    write(1, NEW_REQUEST_MSG, strlen(NEW_REQUEST_MSG));
    write_log("new order\n", user_data);

}

Food* find_food_by_name(Menu* menu, char* food_name){
    for (int i = 0; i < menu->num_of_foods; i++){
        if (strcmp(menu->foods[i]->name, food_name) == 0){
            return menu->foods[i];
        }
    }
    return NULL;
}

void handle_close_fd_msg(UserData* user_data, int fd){
    for (int i = 0; i < restaurant_data.num_of_orders; i++){
        if (restaurant_data.orders[i].fd == fd && restaurant_data.orders[i].status == PENDING){
            restaurant_data.orders[i].status = TIMEOUT;
            write_log("order timeout\n", user_data);
        }
    }
}

void msg_handler(char* msg, UserData* user_data, int fd){
    if (!restaurant_data.is_open)
        return;
    MsgData msg_data = parse_msg(msg);
    const char* msg_title = msg_data.title;

    if (strcmp(msg_title, "new") == 0){
        handle_new_user_msg(msg_data, user_data);
    }

    if (strcmp(msg_title, "get-restaurants") == 0){
        handle_get_restaurants_msg(msg_data, user_data);
    }

    if (strcmp(msg_title, "post-supplier") == 0){
        handle_post_supplier_msg(msg_data, user_data);
    }

    if (strcmp(msg_title, "order-food") == 0){
        handle_order_food_msg(msg_data, user_data, fd);
    }

    if (strcmp(msg_title, "close-fd") == 0){
        handle_close_fd_msg(user_data, fd);
    }


}

void handle_show_recipes_cmd(UserData* user_data){
    for (int i = 0; i < user_data->menu->num_of_foods; i++){
        const char* cur_food_name = user_data->menu->foods[i]->name;
        char* msg_format = "%d- %s\n";
        char* msg = (char*)malloc(strlen(msg_format) + strlen(cur_food_name) + 1) + 1;
        sprintf(msg, msg_format, i + 1, cur_food_name);
        write(1, msg, strlen(msg));
        for (int j = 0; j < user_data->menu->foods[i]->num_of_ingredients; j++){
            const char* cur_ingredient_name = user_data->menu->foods[i]->ingredients[j].name;
            const int cur_ingredient_size = user_data->menu->foods[i]->ingredients[j].quantity;
            char* msg_format = "\t %s : %d\n";
            char* msg = (char*)malloc(strlen(msg_format) + strlen(cur_food_name) + MAX_INDEX_LENGTH - 2);
            sprintf(msg, msg_format, cur_ingredient_name, cur_ingredient_size);
            write(1, msg, strlen(msg));
        }
    }
    write_log("show recipe request sent.\n", user_data);

}

Order* find_order_by_port(int port){
    for (int i = 0; i < restaurant_data.num_of_orders; i++){
        if (restaurant_data.orders[i].port == port)
            return &(restaurant_data.orders[i]);
    }
    return NULL;
}

void answer_request(UserData* user_data){
    write(1, ASK_PORT_COSTUMER, strlen(ASK_PORT_COSTUMER));
    char port_str[PORT_LENGTH];
    read(1, port_str, PORT_LENGTH);
    int port = atoi(port_str);
    Order* order = find_order_by_port(port);
    if (order == NULL){
        write(1, NO_REQUEST_FOUND, strlen(NO_REQUEST_FOUND));
        return;
    }
    const char* msg = "your answer: (yes/no)? ";
    write(1, msg, strlen(msg));
    char ans[4];
    int ans_len = read(0, ans, 4);
    ans[ans_len - 1] = '\0';
    if (strcmp(ans, ACCEPT_RES) == 0){
        make_food(order->food);
        order->status = ACCEPTED;
        write_log("request accepted\n", user_data);
    }
    else{
        order->status = DENIED;
        write_log("request denied\n", user_data);
    }
    ans[ans_len - 1] = '\0';
    send_tcp_msg(order->fd, ans, MAX_SEND_TRIES);

}

void handle_show_suppliers_cmd(UserData* user_data){
    const char* msg_format =
            "get-suppliers\n"                // title
            "port %d\n";                      // port
    size_t msg_size = strlen(msg_format) + PORT_LENGTH - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, user_data->tcp_port, user_data->username);
    broadcast_msg(user_data->broadcast_port, msg);
    write(1, USERNAME_PORT, strlen(USERNAME_PORT));
    write_log("show suppliers request sent.\n", user_data);

}

void handle_show_ingredients_cmd(UserData* user_data){
    write(1, "ingredient/amount\n", strlen("ingredient/amount\n"));
    for (int i = 0; i < restaurant_data.num_of_ingredients; i++){
        if (restaurant_data.ingredients[i].quantity == 0)
            continue;
        char* msg_format = "%s/%d\n";
        char* msg = (char*)malloc(strlen(msg_format) + strlen(restaurant_data.ingredients[i].name) + MAX_INDEX_LENGTH - 2);
        sprintf(msg, msg_format, restaurant_data.ingredients[i].name, restaurant_data.ingredients[i].quantity);
        write(1, msg, strlen(msg));
    }
    write_log("show ingredients cmd\n", user_data);
}

void increase_ingredient(char* ingredient_name, int amount){
    int index = find_ingredient_index(ingredient_name);
    if (index < 0)
        return;
    restaurant_data.ingredients[index].quantity += amount;
}

void handle_request_ingredient_cmd(UserData* user_data){
    write(1, ASK_PORT_SUPPLIER, strlen(ASK_PORT_SUPPLIER));
    char port_str[PORT_LENGTH];
    read(1, port_str, PORT_LENGTH);
    write(1, ASK_INGREDIENT_NAME, strlen(ASK_INGREDIENT_NAME));
    char ingredient_name[MAX_NAME_SIZE];
    const int name_size = read(1, ingredient_name, sizeof(ingredient_name));
    ingredient_name[name_size - 1] = '\0';
    write(1, ASK_INGREDIENT_NUM, strlen(ASK_INGREDIENT_NUM));
    char ingredient_num_str[MAX_INDEX_LENGTH];
    const int ingredient_num_size = read(1, ingredient_num_str, sizeof(ingredient_num_str));
    //write(1, ingredient_num_str, strlen(ingredient_num_str));
    int ingredient_num = atoi(ingredient_num_str);
    int supplier_port = atoi(port_str);
    const char* msg_format =
            "get-ingredient\n"                // title
            "name %s\n"                       // ingredient name
            "number %d\n";                       // ingredient number
    size_t msg_size = strlen(msg_format) + name_size + ingredient_num_size - 2;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, ingredient_name, ingredient_num);
    write(1, msg, strlen(msg));
    write(1, WAITING_RESPONSE_MSG, strlen(WAITING_RESPONSE_MSG));
    write_log("ingredient request sent\n", user_data);
    signal(SIGALRM, show_timeout_msg);
    siginterrupt(SIGALRM, 1);
    alarm(REQUEST_MAX_WAIT_SENDER);
    const int fd = connect_tcp_client(supplier_port);
    send_tcp_msg(fd, msg, MAX_SEND_TRIES);
    const char* response = receive_tcp(fd);
    alarm(0);
    if (strcmp(response, REJECT_RES) == 0){
        write(1, REQUEST_DENIED, strlen(REQUEST_DENIED));
        write_log(REQUEST_DENIED, user_data);
    }
    else if(strcmp(response, ACCEPT_RES) == 0){
        increase_ingredient(ingredient_name, ingredient_num);
        write(1, REQUEST_ACCEPTED, strlen(REQUEST_ACCEPTED));
        write_log(REQUEST_ACCEPTED, user_data);
    }
    else if(strcmp(response, "busy") == 0){
        write(1, SUPPLIER_BUSY_MSG, strlen(SUPPLIER_BUSY_MSG));
        write_log(SUPPLIER_BUSY_MSG, user_data);
    }
    else{
        write_log(REQUEST_TIMEOUT_LOG, user_data);
    }
    
}

void handle_show_requests_cmd(UserData* user_data){
    write(1, "username/port/food\n", strlen("username/port/food\n"));
    for (int i = 0; i < restaurant_data.num_of_orders; i++){
        Order cur_order = restaurant_data.orders[i];
        if (cur_order.status != PENDING)
            continue;
        char* msg_format = "%s %d %s\n";
        int msg_size = strlen(msg_format) + strlen(cur_order.food->name) + strlen(cur_order.username) + MAX_INDEX_LENGTH - 3;
        char* msg = (char*)malloc(msg_size);
        sprintf(msg, msg_format, cur_order.username, cur_order.port, cur_order.food->name);
        write(1, msg, strlen(msg));
    }
    write_log("show request cmd\n", user_data);

}

void handle_show_sales_cmd(UserData* user_data){
    write(1, "username/order/result\n", strlen("username/order/result\n"));
    for (int i = 0; i < restaurant_data.num_of_orders; i++){
        Order cur_order = restaurant_data.orders[i];
        if (cur_order.status == PENDING)
            continue;
        char* msg_format = "%s %s %s\n";
        int msg_size = strlen(msg_format) + strlen(cur_order.food->name) + strlen(cur_order.username) + MAX_INDEX_LENGTH - 3;
        char* msg = (char*)malloc(msg_size);
        const char* result = (cur_order.status == ACCEPTED) ? "accepted" :
                             (cur_order.status == DENIED) ? "denied" : "timeout";
        sprintf(msg, msg_format, cur_order.username, cur_order.food->name, result);
        write(1, msg, strlen(msg));       
    }
    write_log("show sales cmd\n", user_data);

}

void cmd_handler(char* cmd, UserData* user_data){
    char** cmd_args = parse_cmd(cmd);
    char* cmd_name = cmd_args[0];

    if (strcmp(cmd_name, "start") == 0){
        start_restaurant(user_data);
    }

    else if(strcmp(cmd_name, "break") == 0){
        close_restaurant(user_data);
    }

    else if(!restaurant_data.is_open)
        return;
    
    else if(strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "recipes") == 0){
        handle_show_recipes_cmd(user_data);
    }

    else if(strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "suppliers") == 0){
        handle_show_suppliers_cmd(user_data);
    }

    else if(strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "ingredients") == 0){
        handle_show_ingredients_cmd(user_data);
    }

    else if(strcmp(cmd_name, "request") == 0 && strcmp(cmd_args[1], "ingredient") == 0){
        handle_request_ingredient_cmd(user_data);
    }

    else if(strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "requests") == 0){
        handle_show_requests_cmd(user_data);
    }

    else if(strcmp(cmd_name, "show") == 0 && strcmp(cmd_args[1], "sales") == 0){
        handle_show_sales_cmd(user_data);
    }

    if (strcmp(cmd_name, "answer") == 0 && strcmp(cmd_args[1], "request") == 0){
        answer_request(user_data);
    }
}

int main(int argc, char *argv[]){
    UserData user_data = init_user(atoi(argv[1]));
    init_restaurant_data(&user_data);
    start_restaurant(&user_data);
    handle_connections(&user_data, msg_handler, cmd_handler);
}