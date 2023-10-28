#include "udp.h"

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


int make_udp_socket(){
    int broadcast = 1, reuse_port = 1;

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port));
    return sock_fd;
}

struct sockaddr_in connect_udp(const int port, const int sock_fd) {
    struct sockaddr_in bc_address;
    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr(UDP_IP);

    bind(sock_fd, (struct sockaddr*)&bc_address, sizeof(bc_address));

    return bc_address;
}

void broadcast_msg(const int port, const char* msg) {
    const int sock_fd = make_udp_socket();
    struct sockaddr_in bc_address = connect_udp(port, sock_fd);
    
    if (sendto(sock_fd, msg, strlen(msg), 0, (struct sockaddr*)&bc_address, sizeof(bc_address)) == -1) {
       // log_err(STDERR_FILENO, BROADCAST_FAILED, 1, 1);
        exit(EXIT_FAILURE);
    }
    close(sock_fd);
}

char* receive_udp(int sock_fd) {
    char buf[1024] = {0};

    char* result = NULL;
    int total_bytes_rcvd = 0;
    bool receiving = true;

    while (receiving) {
        int recv_bytes = recv(sock_fd, buf, 1024, 0);
        if (recv_bytes <= 0)
            return result;
        result = (char*)realloc(result, recv_bytes);
        memcpy(result, buf, recv_bytes);
        total_bytes_rcvd += recv_bytes;
        if (recv_bytes < 1024)
            receiving = false;
        fd_set read_fd_set;
        FD_ZERO(&read_fd_set);
        FD_SET(sock_fd, &read_fd_set);
        struct timeval t = {0, 0};
        select(sock_fd, &read_fd_set, NULL, NULL, &t);
        if (!FD_ISSET(sock_fd, &read_fd_set))
            receiving = false;
    }
    result = (char*)realloc(result, total_bytes_rcvd + 1);
    result[total_bytes_rcvd] = '\0';

    return result;
}