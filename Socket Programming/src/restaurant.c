#include "udp.c"
#include "consts.h"

int main(int argc, char *argv[]){
    const int broadcast_port = atoi(argv[1]);
    const char* name_msg = "Enter your restaurant name:\n";
    write(1, name_msg, strlen(name_msg));
    char name[MAX_NAME_SIZE];
    const int name_length = read(0, name, sizeof(name));
    name[name_length - 1] = '\0';
    char msg[2048];
    sprintf(msg, "Restaurant %s opened.\n", name);
    broadcast_msg(broadcast_port, msg);
    write(1, BROADCAST_SUCCESS_MSG, strlen(BROADCAST_SUCCESS_MSG));
}