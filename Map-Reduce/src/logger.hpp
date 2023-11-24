#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

class Logger{
    public:
        Logger(std::string logger_name);
        void log_info(std::string msg);
        void log_error(std::string msg);
        void log_error();
    private:
        std::string logger_name;
};

#endif