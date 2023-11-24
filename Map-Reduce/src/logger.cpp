#include "logger.hpp"
#include "color.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>


Logger::Logger(std::string logger_name){
    this->logger_name = logger_name;
}

Logger::Logger(){

}

void Logger::set_logger_name(std::string logger_name){
    this->logger_name = logger_name;
}

void Logger::log_msg(const std::string& msg_fmt, va_list& args){
    vfprintf(stderr, msg_fmt.c_str(), args);
}

void Logger::log_info(const std::string& msg_fmt, ...){
    std::cerr << Color::GRN << "[INFO] " << this->logger_name << ": ";
    va_list args;
    va_start(args, msg_fmt);
    log_msg(msg_fmt, args);
    va_end(args);
    std::cerr << Color::RST << "\n";
}

void Logger::log_error(const std::string& msg_fmt, ...){
    std::cerr << Color::RED << "[ERROR] " << this->logger_name << ": ";
    va_list args;
    va_start(args, msg_fmt);
    log_msg(msg_fmt, args);
    va_end(args);
    std::cerr << Color::RST << "\n";
}

void Logger::log_error(){
    std::string error = strerror(errno);
    log_error(error);
}

