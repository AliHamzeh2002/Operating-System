#include "logger.hpp"
#include "csvhandler.hpp"
#include "consts.hpp"

#include <iostream>

static Logger logger;


std::vector<double> calc_total_consumption_per_hour(CSV_Handler& csv_handler, int month){
    std::vector<double> total_consumption_per_hour(NUM_HOURS, 0);
    int days = 0;
    int i = 0;
    while (days != 30){
        std::string cur_month = csv_handler.get_cell("Month", i++);
        
        if (std::stoi(cur_month) != month)
            continue;
        for (int j = 0; j < NUM_HOURS; j++){
            std::string cur_consumption = csv_handler.get_cell(std::to_string(j), i);
            total_consumption_per_hour[j] += std::stod(cur_consumption);
        }
        days++;
    }
    return total_consumption_per_hour;
}

void show_report(CSV_Handler& csv_handler, int month, std::string resource){
    std::vector<double> total_consumption_per_hour = calc_total_consumption_per_hour(csv_handler, month);
    std::cout << resource << "\n";
    for (int i = 0; i < NUM_HOURS; i++){
        std::cout << total_consumption_per_hour[i] << "\n";
    }
}

int main(){
    std::string building_name, building_path, resource, month_str;
    std::cin >> building_name >> building_path >> resource >> month_str;
    logger.set_logger_name(building_name + "-" + resource);
    logger.log_info("Resource process started");
    int month = std::stoi(month_str);
    std::string csv_path = building_path + "/" + resource + ".csv";
    CSV_Handler csv_handler(csv_path, logger);
    logger.log_info("csv_handler created");
    show_report(csv_handler, month, resource);
    logger.log_info("Report written");

}