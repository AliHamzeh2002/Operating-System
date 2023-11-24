#include <iostream>
#include <filesystem>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "logger.hpp"
#include "utils.cpp"

static std::vector<std::string> RECOURSES = {"Gas", "Electricity", "Water"};
const static char* BUILDING_EXECUTABLE = "./building.out";

static Logger logger("main");

std::vector<std::string> find_buildings(std::string starting_path){
    if (!std::filesystem::exists(starting_path)){
        logger.log_error("Path does not exist");
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> buildings;
    for (const auto & entry : std::filesystem::directory_iterator(starting_path)){
        if (entry.is_directory()){
            std::string building_name = entry.path().filename();
            buildings.push_back(building_name);
        }
    }
    return buildings;
}

std::vector<std::string> read_space_separated_input(std::string msg){
    std::cout << msg << std::endl;
    std::string input;
    getline(std::cin, input);
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string cur_token;
    while (getline(iss, cur_token, ' ')){
        tokens.push_back(cur_token);
    }
    return tokens;

}

void print_vector(std::string name, std::vector<std::string> vec){
    std::cout << name << ": " << std::endl;
    for (int i = 0; i < vec.size(); i++){
        std::cout << "\t" << i + 1 << "- " << vec[i] << std::endl;
    }
}

std::vector<std::string> make_fifo_files(std::vector<std::string> wanted_buildings){
    std::vector<std::string> fifo_files;
    for (int i = 0; i < wanted_buildings.size(); i++){
        std::string fifo_file = "/tmp/" + wanted_buildings[i] + ".fifo";
        std::cout << fifo_file << std::endl;
        if (mkfifo(fifo_file.c_str(), 0666) == -1){
            logger.log_error("Failed to make fifo file");
            exit(EXIT_FAILURE);
        }
        fifo_files.push_back(fifo_file);
    }
    return fifo_files;
}

std::vector<pid_t> run_buildings_processes(std::string starting_path, int month, std::vector<std::string> wanted_buildings, std::vector<std::string> wanted_resources){
    std::vector<pid_t> children_pids;
    for (auto building_name : wanted_buildings){
        int write_pipe;
        run_new_process(BUILDING_EXECUTABLE, write_pipe, logger);
        int num_of_resources = wanted_resources.size();
        
        std::string pipe_data = starting_path + " " + building_name + " " + std::to_string(month) + " " + std::to_string(num_of_resources);
        for (auto resource : wanted_resources){
            pipe_data += " " + resource;
        }
        write(write_pipe, pipe_data.c_str(), pipe_data.size());
        std::cout << "Writing to pipe: " << pipe_data << std::endl;
        close(write_pipe);
        logger.log_info("Building %s process made", building_name.c_str());
    }
    return children_pids;
}

void remove_fifo_files(std::vector<std::string>& fifo_files){
    for (auto fifo_file : fifo_files){
        if (remove(fifo_file.c_str()) == -1){
            logger.log_error("Failed to remove fifo file");
            exit(EXIT_FAILURE);
        }
    }
}

void run_workers(std::string starting_path, int month, std::vector<std::string> wanted_buildings, std::vector<std::string> wanted_resources){
    std::vector<pid_t> children_pids;
    std::vector<std::string> fifo_files = make_fifo_files(wanted_buildings);
    children_pids = run_buildings_processes(starting_path, month, wanted_buildings, wanted_resources);
    wait_for_children(children_pids, logger);
    remove_fifo_files(fifo_files);
}

int main(int argc, char* argv[]){
    logger.log_info("Main process started");
    if (argc < 2){
        logger.log_error("No starting path provided");
        return EXIT_FAILURE;
    }
    std::string starting_path = argv[1];
    std::vector<std::string> buildings = find_buildings(starting_path);
    print_vector("Buildings", buildings);
    std::vector<std::string> wanted_buildings = read_space_separated_input("Enter the name of buildings you want to process:");
    print_vector("Recourses", RECOURSES);
    std::vector<std::string> wanted_resources = read_space_separated_input("Enter the name of resources you want to process:");
    int month;
    std::cout << "Enter the month you want to process: ";
    std::cin >> month;
    run_workers(starting_path, month, wanted_buildings, wanted_resources);

    return EXIT_SUCCESS;
}