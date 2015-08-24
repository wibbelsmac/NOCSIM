#ifndef DEFINES_HPP
#define DEFINES_HPP

#include <iostream>
#include <fstream>

extern std::ofstream ERROR_FILE;
extern std::ofstream LOG_FILE;
extern std::ofstream RESULTS_FILE;

#define LOCATION __FILE__ << ": " <<  __LINE__ << ": "
#define LOG_ERROR ERROR_FILE << LOCATION
#define LOG_DEBUG LOG_FILE << LOCATION

//extern int INJECTION_RATE;
//extern int MESSAGE_SIZE;
//extern int MESSAGES_IN_SIMULATION;
//extern int MESSAGES_IN_WARMUP;
//extern int MEASSAGES_IN_MEASUREMENT;
extern int SHORT_HOP_COST;
extern int LONG_HOP_COST;
extern int ROUTE_MODE; // either XY or ZIG_ZAG
extern double AGGREGATION_LEVEL;
extern double BIAS_PARAM;
extern double TIME_UNITS_PER_SEC;
extern double TIME_UINTS_PER_MESSAGE_GENERATION_PERIOD;
extern int BYTES_PER_FLIT;
extern std::string ERROR_FILENAME;
extern std::string LOG_FILENAME;
extern std::string RESULTS_FILENAME;
extern std::string SIMULATION_IDENTIFIER;
extern std::string TOPOLOGY;
extern std::string TRAFFIC_PATTERN;

const int WIDTH = 8;
const int HEIGHT = 8;


#endif
