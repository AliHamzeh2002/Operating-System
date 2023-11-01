#include "user.h"


void alarm_handler(int sig){
    return;
}

void show_timeout_msg(int sig){
    write(1, TIMEOUT_MSG, strlen(TIMEOUT_MSG));
}

UserData init_user(const int broadcast_port){
    UserData user_data;
    user_data.tcp_port = find_unused_port();
    user_data.broadcast_port = broadcast_port;
    user_data.udp_fd = make_udp_socket();
    connect_udp(broadcast_port, user_data.udp_fd);
    user_data.tcp_fd = connect_tcp_server(user_data.tcp_port);
    set_username(&user_data);
    user_data.menu = read_json(RECIPE_FILE_NAME);
    const char* file_name_format = "./logs/%s.txt";
    char* file_name = (char*)malloc(strlen(user_data.username) + strlen(file_name_format));
    sprintf(file_name, file_name_format, user_data.username);
    user_data.log_fd = open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    write_log(USERNAME_SET_MSG, &user_data);
    return user_data;
}

void write_log(char* msg, UserData* user_data){
    write(user_data->log_fd, msg, strlen(msg));
}


void set_username(UserData* user_data){
        const char* input_name_msg = INPUT_RESTAURANT_NAME_MSG;
        bool name_set = false;

        char* username = (char*)malloc(MAX_NAME_SIZE * sizeof(char));
        assert(username != NULL);
        write(1, input_name_msg, strlen(input_name_msg));

        while (!name_set){
            const int name_length = read(0, username, sizeof(username));
            username[name_length - 1] = '\0';
            const char* msg_format =
                "new\n"                // title
                "port %d\n"       // port
                "username %s\n";       // username
            size_t msg_size = strlen(msg_format) + strlen(username) + PORT_LENGTH - 2;
            char* msg = (char*)malloc(msg_size * sizeof(char));
            assert(msg != NULL);
            sprintf(msg, msg_format, user_data->tcp_port, username);
            broadcast_msg(user_data->broadcast_port, msg);
            write(1, BROADCAST_SUCCESS_MSG, strlen(BROADCAST_SUCCESS_MSG));
            signal(SIGALRM, alarm_handler);
            siginterrupt(SIGALRM, 1);
            alarm(USERNAME_VERIFY_WAIT);
            const int tcp_accept_fd = accept_tcp_client(user_data->tcp_fd);
            alarm(0);
            if (tcp_accept_fd  < 0)
                break;
            const char* again_msg = "write your username again:\n";
            write(1, again_msg, strlen(again_msg));
        }
    user_data->username = username;
    write(1, USERNAME_SET_MSG, strlen(USERNAME_SET_MSG));
}

void handle_connections(UserData* user_data, void (*msg_handler)(char*, UserData*, int), void (*cmd_handler)(char*, UserData*)){

    fd_set working_set, master_set;
    int new_socket, max_sd;

    FD_ZERO(&master_set);
    FD_SET(STDIN_FILENO, &master_set);    
    FD_SET(user_data->tcp_fd, &master_set);
    FD_SET(user_data->udp_fd, &master_set);
    max_sd = (user_data->udp_fd > user_data->tcp_fd) ? user_data->udp_fd : user_data->tcp_fd;
    while (1) {

        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
        

        for (int cur_fd = 0; cur_fd <= max_sd; cur_fd++) {

            if (!FD_ISSET(cur_fd, &working_set)) 
                continue;
            
            if (cur_fd == STDIN_FILENO){
                char input[MAX_INPUT_SIZE];
                const int input_length = read(STDIN_FILENO, input, sizeof(input));
                input[input_length - 1] = '\0';
                //write(1, input, strlen(input));
                cmd_handler(input, user_data);

            }

            else if (cur_fd == user_data->tcp_fd) {  // new tcp connection
                new_socket = accept_tcp_client(user_data->tcp_fd);
                FD_SET(new_socket, &master_set);
                if (new_socket > max_sd)
                    max_sd = new_socket;
            }

            else if (cur_fd == user_data->udp_fd){ // new broadcast message
                char* rcv_msg = receive_udp(user_data->udp_fd);
                //write(1, rcv_msg, strlen(rcv_msg));
                msg_handler(rcv_msg, user_data, user_data->udp_fd);
            }
                
            else { // new tcp message
                char* rcv_msg = receive_tcp(cur_fd);
                //write(1, rcv_msg, strlen(rcv_msg));
                
                if (rcv_msg == NULL) { // EOF
                    msg_handler("close-fd", user_data, cur_fd);
                    close(cur_fd);
                    FD_CLR(cur_fd, &master_set);
                    continue;
                }

                msg_handler(rcv_msg, user_data, cur_fd);
            }
            
        }
    }
}

void handle_new_user_msg(MsgData msg_data, UserData* user_data){
    const int port = atoi(find_argument(msg_data, "port"));
    const char* username = find_argument(msg_data, "username");
    if (strcmp(username, user_data->username) != 0 || port == user_data->tcp_port){
        if (port != user_data->tcp_port)
            write_log("new user entered\n", user_data);
        return;
    }
    connect_tcp_client(port);
}

