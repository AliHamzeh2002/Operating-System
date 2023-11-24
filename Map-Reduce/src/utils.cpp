#include "logger.hpp"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>



pid_t run_new_process(const char* executable, int& write_pipe, Logger& logger){
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1){
            logger.log_error("Failed to make pipe");
            exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1){
        logger.log_error("Failed to fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0){
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        execl(executable, executable, nullptr);
        logger.log_error("Failed to execute building");
        exit(EXIT_FAILURE);
    }
    else{
        close(pipe_fd[0]);
        write_pipe = pipe_fd[1];
    }
    close(pipe_fd[0]);
    return pid;
}

void wait_for_children(std::vector<pid_t> children_pids, Logger& logger){
    for (auto pid : children_pids){
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