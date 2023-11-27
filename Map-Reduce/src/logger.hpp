#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <cstdarg>

class Logger{
    public:
        Logger(std::string logger_name);
        Logger();
        void set_logger_name(std::string logger_name);
        void log_info(const std::string& msg_fmt, ...);
        void log_error(const std::string& msg_fmt, ...);
        void log_error();
        std::string make_msg(const std::string& msg_fmt, va_list args);

    private:
        std::string logger_name;
};

#endif