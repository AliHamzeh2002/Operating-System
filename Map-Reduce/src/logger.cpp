#include "logger.hpp"
#include "color.hpp"
#include <iostream>
#include <cstring>


Logger::Logger(std::string logger_name){
    this->logger_name = logger_name;
}

void Logger::log_info(std::string msg){
    std::cout << Color::GRN << "[INFO] " << this->logger_name << ": " << msg << Color::RST << std::endl;
}

void Logger::log_error(std::string msg){
    std::cout << Color::RED << "[ERROR] " << this->logger_name << ": " << msg << Color::RST << std::endl;
}

void Logger::log_error(){
    std::string error = strerror(errno);
    log_error(error);
}

