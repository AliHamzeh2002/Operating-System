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

void print_buildings(std::vector<std::filesystem::path> buildings){
    std::cout << "Buildings:" << std::endl;
    for (auto building : buildings){
        std::string building_name = building.filename();
        std::cout << "\t" << building_name << std::endl;
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


    return EXIT_SUCCESS;
}