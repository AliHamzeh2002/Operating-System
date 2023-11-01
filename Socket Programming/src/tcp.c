#include "tcp.h"



int connect_tcp_server(const int port) {
    struct sockaddr_in address;
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    const int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1 ) {
        close(server_fd);
        return -1;
    }

    listen(server_fd, MAX_TCP_CLIENTS);

    return server_fd;
}

int accept_tcp_client(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    client_fd = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&address_len);

    return client_fd;
}

int connect_tcp_client(int port) {
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) { // checking for errors
        return -1;
    }

    return fd;
}

int send_tcp_msg(int sock_fd, const char* msg, int max_tries) {
    int left_bytes = strlen(msg);
    for (int tries = 0; tries <= max_tries && left_bytes > 0; tries++) {
        int sent_bytes = send(sock_fd, msg, left_bytes, 0);
        if (sent_bytes == -1) {
            //log_err(STDERR_FILENO, "tcp message send failed.\n", 1, 1);
            return -1;
        }
        left_bytes -= sent_bytes;
    }
    //if (left_bytes > 0)
        //log_err(STDERR_FILENO, "couldn't send tcp message completly.\n", 1, 1);

    return left_bytes;
}

char* receive_tcp(int sock_fd) {
    int num_bytes = 0, cur_size = 0, total_size = 0;
    char buf[1024] = {0};
    char* result = NULL;
    bool receiving = true;
    while (receiving) {
        num_bytes = recv(sock_fd, buf, 1024, 0);
        if (num_bytes <= 0)
            return result;
        total_size += num_bytes;
        result = (char*)realloc(result, cur_size + num_bytes);
        memcpy(&result[cur_size], buf, num_bytes);
        cur_size += num_bytes;
        if (num_bytes < 1024)
            receiving = false;
        num_bytes = 0;
    }
    result = (char*)realloc(result, total_size + 1);
    result[total_size] = '\0';
    return result;
}

int find_unused_port() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        //perror("Socket creation failed");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Bind to localhost
    addr.sin_port = 0;  // Let the system assign an available port
    
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        //perror("Socket binding failed");
        close(sockfd);
        return -1;
    }
    
    socklen_t addrLen = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addrLen) < 0) {
        //perror("getsockname failed");
        close(sockfd);
        return -1;
    }
    
    int port = ntohs(addr.sin_port);
    close(sockfd);
    
    return port;
}