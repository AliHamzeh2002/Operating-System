#include "logger.hpp"
#include "csvhandler.hpp"

#include <iostream>

static Logger logger;

const int NUM_DAYS = 30;
const int NUM_HOURS = 6;

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

double calc_total_consumption(std::vector<double> total_consumption_per_hour){
    double total_consumption = 0;
    for (int i = 0; i < NUM_HOURS; i++){
        total_consumption += total_consumption_per_hour[i];
    }
    return total_consumption;
}

int find_max_consumption_day(std::vector<double> total_consumption_per_hour){
    int max_consumption_day = 0;
    for (int i = 1; i < NUM_DAYS; i++){
        if (total_consumption_per_hour[i] > total_consumption_per_hour[max_consumption_day])
            max_consumption_day = i;
    }
    return max_consumption_day;
}

void show_report(CSV_Handler& csv_handler, int month, std::string building_name, std::string resource){
    std::vector<double> total_consumption_per_hour = calc_total_consumption_per_hour(csv_handler, month);
    double total_consumption = calc_total_consumption(total_consumption_per_hour);
    double avg_consumption = total_consumption / NUM_DAYS;
    int max_consumption_day = find_max_consumption_day(total_consumption_per_hour);
    double diff_max_avg = total_consumption_per_hour[max_consumption_day] - avg_consumption;
    std::cout << "Building: " << building_name << "\n";
    std::cout << "\t" << "Resource: " << resource << "\n";
    std::cout << "\t\t" <<  "Total consumption: " << total_consumption << "\n";
    std::cout << "\t\t" <<  "Average consumption: " << avg_consumption << "\n";
    std::cout << "\t\t" <<  "Max consumption Day: " << max_consumption_day << "\n";
    std::cout << "\t\t" <<  "Diff max-avg: " << diff_max_avg << "\n";
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
    show_report(csv_handler, month, building_name, resource);
    logger.log_info("Report written");

}