#include <iostream>
#include <vector>

#include "logger.hpp"


static Logger logger("Bills");

std::vector<std::string> get_wanted_buildings(){
    std::vector<std::string> buildings;
    std::string building;
    while (std::cin >> building){
        buildings.push_back(building);
    }
    return buildings;
}

int main(){
    logger.log_info("Bills process started");
    std::string starting_path;
    std::cin >> starting_path;
    std::vector<std::string> buildings = get_wanted_buildings();
    


    return 0;
}
