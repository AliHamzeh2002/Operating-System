#include "logger.hpp"
#include "utils.cpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

const static char* RESOURCE_EXECUTABLE = "./resource.out";
static Logger logger;

std::vector<pid_t> run_resource_processes(std::string building_path, int month, std::vector<std::string> wanted_resources){
    std::vector<pid_t> children_pids;
    for (int i = 0; i < wanted_resources.size(); i++){
        int write_pipe;
        pid_t pid = run_new_process(RESOURCE_EXECUTABLE, write_pipe, logger);
        children_pids.push_back(pid);
        std::string resource = wanted_resources[i];
        std::string month_str = std::to_string(month);
        std::string pipe_data = building_path + " " + resource + " " + month_str;
        write(write_pipe, pipe_data.c_str(), pipe_data.size());
        close(write_pipe);
    }
    return children_pids;
}

int main(){
    std::string starting_path, building_name, month_str, num_of_resources_str;
    std::cin >> starting_path >> building_name >> month_str >> num_of_resources_str;
    logger.set_logger_name(building_name);
    logger.log_info("Building process started");
    int month = std::stoi(month_str);
    int num_of_resources = std::stoi(num_of_resources_str);
    std::vector<std::string> wanted_resources;
    for (int i = 0; i < num_of_resources; i++){
        std::string resource;
        std::cin >> resource;
        wanted_resources.push_back(resource);
    }
    std::string building_path = starting_path + "/" + building_name;
    if (!std::filesystem::exists(building_path)){
        logger.log_error("Building path does not exist");
        exit(EXIT_FAILURE);
    }
    std::vector<pid_t> children_pids = run_resource_processes(building_path, month, wanted_resources);
    wait_for_children(children_pids, logger);
    exit(EXIT_SUCCESS);
}