#ifndef CSV_HANDLER_HPP
#define CSV_HANDLER_HPP

#include "logger.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

using Column = std::pair<std::string, std::vector<std::string>>;

class CSV_Handler{
    public:
        CSV_Handler(std::string path, Logger& logger);
        std::vector<std::string> get_column(std::string key);
        std::string get_cell(std::string key, int index);

    private:
        std::vector<Column> data;
        void set_titles(std::ifstream& file);
        void read_row(std::string row);
        
};

#endif