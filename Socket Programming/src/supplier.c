#include "supplier.h"

void init_supplier_data(){
    supplier_data.ingredient_request.is_valid = false;
}

void delete_request(int sig){
    supplier_data.ingredient_request.is_valid = false;
}

void handle_get_ingredient_msg(MsgData msg_data, UserData* user_data, int fd){
    if (supplier_data.ingredient_request.is_valid){
        send_tcp_msg(fd, "busy", MAX_SEND_TRIES);
        return;
    }
    char* ingredient_name = find_argument(msg_data, "name");
    const int ingredient_number = atoi(find_argument(msg_data, "number"));
    IngredientRequest new_request;
    new_request.connection_fd = fd;
    new_request.ingredient.name = (char*)malloc(sizeof(char) * strlen(ingredient_name));
    strcpy(new_request.ingredient.name, ingredient_name);
    new_request.ingredient.quantity = ingredient_number;
    new_request.is_valid = true;
    supplier_data.ingredient_request = new_request; 
    write_log(NEW_REQUEST_MSG, user_data);
    write(1, NEW_REQUEST_MSG, strlen(NEW_REQUEST_MSG));
    signal(SIGALRM, delete_request);
    siginterrupt(SIGALRM, 1);
    alarm(REQUEST_MAX_WAIT_RECEIVER);
}

void handle_get_suppliers_msg(MsgData msg_data, UserData* user_data){
    int fd = connect_tcp_client(atoi(find_argument(msg_data, "port")));
    const char* msg_format =
            "post-supplier\n"                // title
            "port %d\n"                        // port
            "name %s\n";                   // name
    const int msg_size = strlen(msg_format) + strlen(user_data->username) + PORT_LENGTH - 1;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    
    assert(msg != NULL);
    sprintf(msg, msg_format, user_data->tcp_port, user_data->username);
    send_tcp_msg(fd, msg, MAX_SEND_TRIES);
    write_log(INFO_ASKED_LOG, user_data);
}

void answer_request(UserData* user_data){
    if (!supplier_data.ingredient_request.is_valid){
        write(1, NO_REQUEST_MSG, strlen(NO_REQUEST_MSG));
        return;
    }
    const char* msg_format = "request: %s %d accept(yes/no)? ";
    const int msg_size = strlen(msg_format) + strlen(supplier_data.ingredient_request.ingredient.name) + MAX_INDEX_LENGTH;
    char* msg = (char*)malloc(msg_size * sizeof(char));
    assert(msg != NULL);
    sprintf(msg, msg_format, supplier_data.ingredient_request.ingredient.name, supplier_data.ingredient_request.ingredient.quantity);
    write(1, msg, strlen(msg));
    char ans[4];
    int ans_len = read(0, ans, 4);
    ans[ans_len - 1] = '\0';
    char* msg_log = (ans == ACCEPT_RES) ? REQUEST_ACCEPTED : REQUEST_DENIED;
    write_log(REQUEST_ACCEPTED, user_data);
    send_tcp_msg(supplier_data.ingredient_request.connection_fd, ans, MAX_SEND_TRIES);
}

void msg_handler(char* msg, UserData* user_data, int fd){
    MsgData msg_data = parse_msg(msg);
    const char* msg_title = msg_data.title;

    if (strcmp(msg_title, "new") == 0){
        handle_new_user_msg(msg_data, user_data);
    }

    else if (strcmp(msg_title, "get-suppliers") == 0){
        handle_get_suppliers_msg(msg_data, user_data);
    }

    else if (strcmp(msg_title, "get-ingredient") == 0){
       handle_get_ingredient_msg(msg_data, user_data, fd);
    }
}

void cmd_handler(char* cmd, UserData* user_data){
    char** cmd_args = parse_cmd(cmd);
    char* cmd_name = cmd_args[0];

    if (strcmp(cmd_name, "answer") == 0 && strcmp(cmd_args[1], "request") == 0){
        answer_request(user_data);
    }
}


int main(int argc, char *argv[]){
    UserData user_data = init_user(atoi(argv[1]));
    init_supplier_data();
    handle_connections(&user_data, msg_handler, cmd_handler);
}