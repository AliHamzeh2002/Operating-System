#include <iostream>
#include <filesystem>
#include <vector>
#include "logger.hpp"

static Logger logger("main");

std::vector<std::filesystem::path> find_buildings(std::string starting_path){
    if (!std::filesystem::exists(starting_path)){
        logger.log_error("Path does not exist");
        exit(EXIT_FAILURE);
    }
    std::vector<std::filesystem::path> buildings;
    for (const auto & entry : std::filesystem::directory_iterator(starting_path)){
        if (entry.is_directory()){
            buildings.push_back(entry.path());
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

void print_buildings(std::vector<std::filesystem::path> buildings){
    std::cout << "Buildings:" << std::endl;
    for (int i = 0; i < buildings.size(); i++){
        std::filesystem::path building = buildings[i];
        std::string building_name = building.filename();
        std::cout << "\t" << i + 1 << "- " << building_name << std::endl;
    }
}

int main(int argc, char* argv[]){
    logger.log_info("Starting program");
    if (argc < 2){
        logger.log_error("No starting path provided");
        return EXIT_FAILURE;
    }
    std::string starting_path = argv[1];
    std::vector<std::filesystem::path> buildings = find_buildings(starting_path);
    print_buildings(buildings);
    std::vector<std::string> wanted_buildings = read_space_separated_input("Enter the name of buildings you want to process:");
    std::vector<std::string> wanted_resources = read_space_separated_input("Enter the name of resources you want to process:");



    return EXIT_SUCCESS;
}