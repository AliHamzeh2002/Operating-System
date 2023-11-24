#include "logger.hpp"

#include <iostream>

static Logger logger;

int main(){
    logger.set_logger_name("resource");
    logger.log_info("Resource process started");
    
}