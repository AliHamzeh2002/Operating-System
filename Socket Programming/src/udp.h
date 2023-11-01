#ifndef UDP_H

#define UDP_H

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <asm/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "consts.h"

int make_udp_socket();
struct sockaddr_in connect_udp(const int port, const int sock_fd);
void broadcast_msg(const int port, const char* msg);
char* receive_udp(const int sock_fd);



#endif
