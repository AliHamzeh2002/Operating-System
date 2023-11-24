#include "csvhandler.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

CSV_Handler::CSV_Handler(std::string path, Logger& logger){
    std::ifstream file(path);
    if (!file.is_open()){
        logger.log_error("Failed to open the CSV file.");
        exit(EXIT_FAILURE);
    }
    set_titles(file);
    std::string line;
    while (std::getline(file, line)) {
        read_row(line);
    }
    file.close();


}

void CSV_Handler::set_titles(std::ifstream& file){
    std::string line;
    std::getline(file, line);
    std::istringstream ss(line);
    std::string title;
    while (std::getline(ss, title, ',')){
        data.push_back({title, std::vector<std::string>()});
    }    
}

void CSV_Handler::read_row(std::string row){
    std::istringstream ss(row);
    std::string cell;
    for (auto& column : data){
        std::getline(ss, cell, ',');
        column.second.push_back(cell);
    }
}

std::vector<std::string> CSV_Handler::get_column(std::string key){
    for (auto& column : data){
        if (column.first == key){
            return column.second;
        }
    }
    return std::vector<std::string>();
}

std::string CSV_Handler::get_cell(std::string key, int index){
    return get_column(key)[index];
}