#include <iostream>
#include <filesystem>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "logger.hpp"
#include "utils.cpp"
#include "consts.hpp"



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
    std::cout << msg;
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
    std::cout << name << ": " << "\n";
    for (int i = 0; i < vec.size(); i++){
        std::cout << "\t" << i + 1 << "- " << vec[i] << "\n";
    }
}



std::vector<std::string> make_fifo_files(std::vector<std::string> wanted_buildings){
    std::vector<std::string> fifo_files;
    for (int i = 0; i < wanted_buildings.size(); i++){
        std::string fifo_file = "/tmp/" + wanted_buildings[i] + ".fifo";
        if (mkfifo(fifo_file.c_str(), 0666) == -1){
            logger.log_error("Failed to make fifo file");
            exit(EXIT_FAILURE);
        }
        fifo_files.push_back(fifo_file);
    }
    return fifo_files;
}

std::vector<ChildData> run_buildings_processes(std::string starting_path, int month, std::vector<std::string> wanted_buildings,
                                               std::vector<std::string> wanted_resources, std::vector<std::string> wanted_measures){
    std::vector<ChildData> children_data;
    for (auto building_name : wanted_buildings){
        int write_pipe;
        int read_pipe;
        pid_t child_pid = run_new_process(BUILDING_EXECUTABLE, write_pipe, read_pipe, logger);
        int num_of_resources = wanted_resources.size();
        std::string pipe_data = starting_path + " " + building_name + " " + std::to_string(month) + " " + std::to_string(num_of_resources);
        for (auto resource : wanted_resources){
            pipe_data += " " + resource;
        }
        for (auto measure : wanted_measures){
            pipe_data += " " + measure;
        }
        write(write_pipe, pipe_data.c_str(), pipe_data.size());
        close(write_pipe);
        children_data.push_back({child_pid, read_pipe});
        logger.log_info("Building %s process made", building_name.c_str());
    }
    return children_data;
}

ChildData run_bills_process(std::string starting_path, int month, std::vector<std::string> wanted_buildings){
    int write_pipe;
    int read_pipe;
    pid_t child_pid = run_new_process(BILLS_EXECUTABLE, write_pipe, read_pipe, logger);
    std::string pipe_data = starting_path;
    pipe_data += " " + std::to_string(month);
    for (auto building_name : wanted_buildings){
        pipe_data += " " + building_name;
    }
    write(write_pipe, pipe_data.c_str(), pipe_data.size());
    close(write_pipe);
    logger.log_info("Bills process made");   
    return {child_pid, read_pipe};
}

void remove_fifo_files(std::vector<std::string>& fifo_files){
    for (auto fifo_file : fifo_files){
        if (remove(fifo_file.c_str()) == -1){
            logger.log_error("Failed to remove fifo file");
            exit(EXIT_FAILURE);
        }
    }
}

void print_children_outputs(std::vector<std::string>& children_outputs){
    for (auto &data : children_outputs){
        std::cout << data;
    }
}

void run_workers(std::string starting_path, int month, std::vector<std::string> wanted_buildings,
                 std::vector<std::string> wanted_resources, std::vector<std::string> wanted_measures){
    std::vector<ChildData> children_data;
    std::vector<std::string> fifo_files = make_fifo_files(wanted_buildings);
    children_data = run_buildings_processes(starting_path, month, wanted_buildings, wanted_resources, wanted_measures);
    ChildData bills_data = run_bills_process(starting_path, month, wanted_buildings);
    children_data.push_back(bills_data);
    wait_for_children(children_data, logger);
    remove_fifo_files(fifo_files);
    std::vector<std::string> children_outputs = read_children_outputs(children_data);
    print_children_outputs(children_outputs);

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
    std::vector<std::string> wanted_buildings = read_space_separated_input("Enter the name of buildings you want to process: ");
    print_vector("Recourses", RECOURSES);
    std::vector<std::string> wanted_resources = read_space_separated_input("Enter the name of resources you want to process: ");
    print_vector("Measures", MEASURES);
    std::vector<std::string> wanted_measures = read_space_separated_input("Enter the index of measures you want to process: ");
    int month;
    std::cout << "Enter the month you want to process: ";
    std::cin >> month;
    run_workers(starting_path, month, wanted_buildings, wanted_resources, wanted_measures);

    return EXIT_SUCCESS;
}

