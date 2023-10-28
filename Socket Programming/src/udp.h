#ifndef UDP_H

#define UDP_H

int make_udp_socket();
struct sockaddr_in connect_udp(const int port, const int sock_fd);
void broadcast_msg(const int port, const char* msg);
char* receive_udp(const int sock_fd);



#endif
