#ifndef CONSTS_HPP
#define CONSTS_HPP
#include <vector>
#include <string>
const int NUM_HOURS = 6;
const int NUM_DAYS = 30;
static std::vector<std::string> RECOURSES = {"Gas", "Electricity", "Water"};
static std::vector<std::string> MEASURES = {
                                                "total consumption",    
                                                "average consumption", 
                                                "max consumption hour",
                                                "max - average consumption",
                                            };
const static char* BUILDING_EXECUTABLE = "./building.out";
const static char* BILLS_EXECUTABLE = "./bills.out";
const static char* RESOURCE_EXECUTABLE = "./resource.out";

#endif