#include <iostream>
#include <filesystem>
#include <vector>

std::vector<std::filesystem::path> find_buildings(std::string starting_path){
    std::vector<std::filesystem::path> buildings;
    for (const auto & entry : std::filesystem::directory_iterator(starting_path)){
        if (entry.is_directory()){
            buildings.push_back(entry.path());
        }
    }
    return buildings;
}

int main(int argc, char* argv[]){
    if (argc < 2){
        return EXIT_FAILURE;
    }
    std::string starting_path = argv[1];
    std::vector<std::filesystem::path> buildings = find_buildings(starting_path);
    //print the last folder of each path without ""
    for (auto building : buildings){
        std::string s = building.filename();
        std::cout << s << std::endl;
    }

    return EXIT_SUCCESS;
}