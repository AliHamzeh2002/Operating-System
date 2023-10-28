#include "udp.c"
#include "consts.h"

int main(int argc, char const *argv[]) {
    const int port = atoi(argv[1]);
    const int sock_fd = make_udp_socket();
    connect_udp(port, sock_fd);
    char* rcv_msg = receive_udp(sock_fd);
    write(1, rcv_msg, strlen(rcv_msg));

    return 0;
}