#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "logger.hpp"
#include "csvhandler.hpp"
#include "color.hpp"


static Logger logger("Bills");

const int NUM_HOURS = 6;

std::vector<std::string> get_wanted_buildings(){
    std::vector<std::string> buildings;
    std::string building;
    while (std::cin >> building){
        buildings.push_back(building);
    }
    return buildings;
}

std::vector<std::string> read_fifos(std::vector<std::string> buildings){
    std::vector<std::string> fifo_data;
    for (int i = 0; i < buildings.size(); i++){
        std::string fifo_file = "/tmp/" + buildings[i] + ".fifo";
        std::ifstream file(fifo_file);
        std::string line;
        std::string building_data;
        while (std::getline(file, line)){
            building_data += line;
        }
        file.close();
        fifo_data.push_back(building_data);
    }
    return fifo_data;
}


std::string generate_water_bill(std::vector<double> consumption_per_hour, int max_consumption_hour, int month, CSV_Handler& csv_handler){
    double bill_price = 0;
    int coeff = stoi(csv_handler.get_cell("water", month - 1));
    for (int i = 1; i <= NUM_HOURS; i++){
        if (i == max_consumption_hour){
            bill_price += consumption_per_hour[i - 1] * 1.25 * coeff;
            continue;
        }
        bill_price += consumption_per_hour[i - 1] * 1.0 * coeff;
    }
    std::string bill = "Water: " + std::to_string(bill_price) + "$\n";
    return bill;
}

std::string generate_gas_bill(std::vector<double> consumption_per_hour, int max_consumption_hour, int month, CSV_Handler& csv_handler){
    double bill_price = 0;
    int coeff = stoi(csv_handler.get_cell("gas", month - 1));
    for (int i = 1; i <= NUM_HOURS; i++){
        bill_price += consumption_per_hour[i - 1] * 1.0 * coeff;
    }
    std::string bill = "Gas: " + std::to_string(bill_price) + "$\n";
    return bill;
}

std::string generate_electricity_bill(std::vector<double> consumption_per_hour, int max_consumption_hour, int month, CSV_Handler& csv_handler){
    double avg = 0;
    for (int i = 0; i < NUM_HOURS; i++){
        avg += consumption_per_hour[i];
    }
    avg /= NUM_HOURS;
    double bill_price = 0;
    int coeff = stoi(csv_handler.get_cell("electricity", month - 1));
    for (int i = 1; i <= NUM_HOURS; i++){
        if (i == max_consumption_hour){
            bill_price += consumption_per_hour[i - 1] * 1.25 * coeff;
            continue;
        }
        if (consumption_per_hour[i - 1] < avg){
            bill_price += consumption_per_hour[i - 1] * 0.75 * coeff;
            continue;
        }
        bill_price += consumption_per_hour[i - 1] * 1.0 * coeff;
    }
    std::string bill = "Electricity: " + std::to_string(bill_price) + "$\n";
    return bill;
}


std::string generate_bill(std::string resource, std::vector<double> consumption_per_hour, int max_consumption_hour, int month, CSV_Handler& csv_handler){
    std::string bill;
    if (resource == "Water")
        bill = generate_water_bill(consumption_per_hour, max_consumption_hour, month, csv_handler);
    else if (resource == "Gas")
        bill = generate_gas_bill(consumption_per_hour, max_consumption_hour, month, csv_handler);
    else if (resource == "Electricity")
        bill = generate_water_bill(consumption_per_hour, max_consumption_hour, month, csv_handler);
    return bill;
}

void handle_fifo_data(std::vector<std::string> fifo_data, std::vector<std::string> buildings, int month, CSV_Handler& csv_handler){
    std::ostringstream bills_ss;
    bills_ss << Color::CYN << "Bills:\n";
    for (int i = 0; i < fifo_data.size(); i++){
        bills_ss << "\tBuilding: " << buildings[i] << "\n";
        std::string data = fifo_data[i];
        std::string building_name = buildings[i];
        std::istringstream ss(data);
        std::string resource;
        while(ss >> resource){
            std::vector<double> consumption_per_hour;
            for (int i = 0; i < NUM_HOURS; i++){
                double consumption;
                ss >> consumption;
                consumption_per_hour.push_back(consumption);
            }
            int max_consumption_hour;
            ss >> max_consumption_hour;

            std::string resource_bill = generate_bill(resource, consumption_per_hour, max_consumption_hour, month, csv_handler);
            bills_ss << "\t\t" << resource_bill;
        }
    }
    std::cout << bills_ss.str();
}

int main(){
    logger.log_info("Bills process started");
    std::string starting_path, month_str;
    std::cin >> starting_path >> month_str;
    int month = std::stoi(month_str);
    std::vector<std::string> buildings = get_wanted_buildings();
    std::vector<std::string> fifo_data = read_fifos(buildings);
    logger.log_info("Fifo data read");
    std::string bills_csv_path = starting_path + "/bills.csv";
    CSV_Handler csv_handler(bills_csv_path, logger);
    handle_fifo_data(fifo_data, buildings, month, csv_handler);
    logger.log_info("Bills written");
    exit(EXIT_SUCCESS);
}
