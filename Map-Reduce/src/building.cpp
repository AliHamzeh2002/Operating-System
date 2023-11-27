#include "logger.hpp"
#include "utils.cpp"
#include "consts.hpp"
#include "color.hpp"
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

static Logger logger;

std::vector<ChildData> run_resource_processes(std::string starting_path, std::string building_name, 
                                            int month, std::vector<std::string> wanted_resources){
    std::string building_path = starting_path + "/" + building_name;
    if (!std::filesystem::exists(building_path)){
        logger.log_error("Building path does not exist");
        exit(EXIT_FAILURE);
    }
    std::vector<ChildData> children_data;
    for (int i = 0; i < wanted_resources.size(); i++){
        int write_pipe;
        int read_pipe;
        pid_t pid = run_new_process(RESOURCE_EXECUTABLE, write_pipe, read_pipe, logger);
        children_data.push_back({pid, read_pipe});
        std::string resource = wanted_resources[i];
        std::string month_str = std::to_string(month);
        std::string pipe_data = building_name + " " + building_path + " " + resource + " " + month_str;
        write(write_pipe, pipe_data.c_str(), pipe_data.size());
        close(write_pipe);
    }
    return children_data;
}

std::string make_fifo_data(std::vector<double> consumption_per_hour, std::string resource, int max_consumption_hour){
    std::string fifo_data = resource + " ";
    for (int i = 0; i < NUM_HOURS; i++){
        fifo_data += std::to_string(consumption_per_hour[i]) + " ";
    }
    fifo_data += std::to_string(max_consumption_hour) + "\n";
    return fifo_data;
}

void send_data_to_bills_process(std::string data, std::string building_name){
    std::string fifo_path = "/tmp/" + building_name + ".fifo";
    int fifo_fd = open(fifo_path.c_str(), O_WRONLY);
    if (fifo_fd == -1){
        logger.log_error("Failed to open fifo file");
        exit(EXIT_FAILURE);
    }
    write(fifo_fd, data.c_str(), data.size());
    close(fifo_fd);
}


std::vector<double> read_consumption_per_hour(std::istringstream& ss){
    std::vector<double> consumption_per_hour;
    for (int i = 0; i < NUM_HOURS; i++){
        std::string consumption;
        ss >> consumption;
        consumption_per_hour.push_back(std::stod(consumption));
    }
    return consumption_per_hour;
}

double calc_total_consumption(std::vector<double> consumption_per_hour){
    double total_consumption = 0;
    for (auto consumption : consumption_per_hour){
        total_consumption += consumption;
    }
    return total_consumption;
}

int calc_max_consumption_hour(std::vector<double> consumption_per_hour){
    int max_consumption_hour = 0;
    for (int i = 1; i < NUM_HOURS; i++){
        if (consumption_per_hour[i] > consumption_per_hour[max_consumption_hour]){
            max_consumption_hour = i;
        }
    }
    return max_consumption_hour;
}

bool is_in_vector(std::vector<std::string> v, std::string s){
    for (auto& elem : v){
        if (elem == s)
            return true;
    }
    return false;
}

void write_resource_report(std::string resource, double total_consumption, double avg_consumption,
                           int max_consumption_hour, double diff_max_avg, std::vector<std::string> wanted_measures){
    std::cout << "\t" << "Resource: " << resource << "\n";
    if (is_in_vector(wanted_measures, "1"))
        std::cout << "\t\t" <<  "Total-consumption: " << total_consumption << "\n";
    if (is_in_vector(wanted_measures, "2"))
        std::cout << "\t\t" <<  "Average-consumption: " << avg_consumption << "\n";
    if (is_in_vector(wanted_measures, "3"))
        std::cout << "\t\t" <<  "Max-consumption Hour: " << max_consumption_hour << "\n";
    if (is_in_vector(wanted_measures, "4"))
        std::cout << "\t\t" <<  "Diff-max-avg: " << diff_max_avg << "\n";
}

void handle_children_outputs(std::vector<ChildData> children_data, std::string building_name,
                             std::vector<std::string> wanted_measures){
    std::cout << Color::MAG << "Building: " << building_name << "\n";
    std::cout << std::fixed << std::setprecision(PRECISION);
    std::vector<std::string> children_outputs = read_children_outputs(children_data);
    std::string fifo_data;
    for (auto& data : children_outputs){
        std::istringstream ss(data);
        std::string resource;
        ss >> resource;
        std::vector<double> consumption_per_hour = read_consumption_per_hour(ss);
        double total_consumption = calc_total_consumption(consumption_per_hour);
        double avg_consumption = total_consumption / (NUM_HOURS * NUM_DAYS);
        int max_consumption_hour = calc_max_consumption_hour(consumption_per_hour);
        double diff_max_avg = consumption_per_hour[max_consumption_hour] / NUM_DAYS - avg_consumption;
        fifo_data = fifo_data + make_fifo_data(consumption_per_hour, resource, max_consumption_hour);
        write_resource_report(resource, total_consumption, avg_consumption,
                             max_consumption_hour, diff_max_avg, wanted_measures);
    }
    logger.log_info("Building data written.");
    send_data_to_bills_process(fifo_data, building_name);
    logger.log_info("Data sent to bills office");

}

int main(){
    std::string starting_path, building_name, month_str, num_of_resources_str;
    std::cin >> starting_path >> building_name >> month_str >> num_of_resources_str;
    logger.set_logger_name(building_name);
    logger.log_info("Building process started");
    int month = std::stoi(month_str);
    int num_of_resources = std::stoi(num_of_resources_str);
    std::vector<std::string> wanted_resources;
    for (int i = 0; i < num_of_resources; i++){
        std::string resource;
        std::cin >> resource;
        wanted_resources.push_back(resource);
    }
    std::string measure;
    std::vector<std::string> wanted_measures;
    while (std::cin >> measure){
        wanted_measures.push_back(measure);
    }
    
    std::vector<ChildData> children_data = run_resource_processes(starting_path, building_name, month, wanted_resources);
    wait_for_children(children_data, logger);
    handle_children_outputs(children_data, building_name, wanted_measures);
    exit(EXIT_SUCCESS);
}


