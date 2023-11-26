#include "logger.hpp"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

using ChildData = std::pair<pid_t, int>;

pid_t run_new_process(const char* executable, int& write_pipe, int& read_pipe,  Logger& logger){
    int pipe1[2];
    if (pipe(pipe1) == -1){
            logger.log_error("Failed to make pipe");
            exit(EXIT_FAILURE);
    }
    int pipe2[2];
    if (pipe(pipe2) == -1){
            logger.log_error("Failed to make pipe");
            exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1){
        logger.log_error("Failed to fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0){
        close(pipe1[1]);
        close(pipe2[0]);
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);  
        close(pipe1[0]);
        close(pipe2[1]);
        execl(executable, executable, nullptr);
        logger.log_error("Failed to execute building");
        exit(EXIT_FAILURE);
    }
    else{
        close(pipe1[0]);
        close(pipe2[1]);
        write_pipe = pipe1[1];
        read_pipe = pipe2[0];
    }
    close(pipe1[0]);
    return pid;

    
}

void wait_for_children(std::vector<ChildData> children_data, Logger& logger){
    for (auto child_data : children_data){
        pid_t pid = child_data.first;
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)){
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0){
                logger.log_error("Child exited with non-zero status");
                exit(EXIT_FAILURE);
            }
        }
        else{
            logger.log_error("Child did not exit normally");
            exit(EXIT_FAILURE);
        }
    }
}

std::vector<std::string> read_children_outputs(std::vector<ChildData> children_data){
    std::vector<std::string> pipe_outputs;
    for (auto child_data : children_data){
        int read_pipe = child_data.second;
        std::string pipe_data;
        char buffer[1024];
        int num_of_bytes;
        while (num_of_bytes = read(read_pipe, buffer, 1024)){
            buffer[num_of_bytes] = '\0';
            pipe_data += buffer;
            //std::cout << buffer;
        }
        pipe_outputs.push_back(pipe_data);
        close(read_pipe);
    }
    return pipe_outputs;
}

