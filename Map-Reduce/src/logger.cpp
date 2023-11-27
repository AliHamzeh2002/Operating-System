#include "logger.hpp"
#include "color.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>


Logger::Logger(std::string logger_name){
    this->logger_name = logger_name;
}

Logger::Logger(){

}

void Logger::set_logger_name(std::string logger_name){
    this->logger_name = logger_name;
}

std::string Logger::make_msg(const std::string& msg_fmt, va_list args){
    char result[4096];
    vsnprintf(result, sizeof(result), msg_fmt.c_str(), args);
    return result;
}

void Logger::log_info(const std::string& msg_fmt, ...){
    std::ostringstream ss;
    ss << Color::GRN << "[INFO] " << this->logger_name << ": ";
    va_list args;
    va_start(args, msg_fmt);
    ss << make_msg(msg_fmt, args);
    va_end(args);
    ss << Color::RST << "\n";
    std::cerr << ss.str();
}

void Logger::log_error(const std::string& msg_fmt, ...){
    std::ostringstream ss;
    ss << Color::RED << "[ERROR] " << this->logger_name << ": ";
    va_list args;
    va_start(args, msg_fmt);
    ss << make_msg(msg_fmt, args);
    va_end(args);
    ss << Color::RST << "\n";
    std::cerr << ss.str();
}

void Logger::log_error(){
    std::string error = strerror(errno);
    log_error(error);
}

