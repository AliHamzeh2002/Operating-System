#include "logger.hpp"
#include <iostream>

static Logger logger;

int main(){
    std::string building_name;
    std::cin >> building_name;
    logger.set_logger_name(building_name);
    logger.log_info("Building started");
}