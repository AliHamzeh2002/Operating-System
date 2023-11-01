#ifndef TCP_H
#define TCP_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "consts.h"

int connect_tcp_server(const int port);

int accept_tcp_client(const int server_fd) ;

int connect_tcp_client(const int port);

int send_tcp_msg(int sock_fd, const char* msg, int max_tries);

char* receive_tcp(int sock_fd);

int find_unused_port();



#endif