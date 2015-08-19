#ifndef ORACLE_TEST_H
#define ORACLE_TEST_H

bool run_simple_hor_cycle_test();
int get_uid();

extern std::ofstream ERROR_FILE;
extern std::ofstream LOG_FILE;
extern std::ofstream RESULTS_FILE;

extern std::string ERROR_FILENAME;
extern std::string LOG_FILENAME;
extern std::string RESULTS_FILENAME;
extern std::string SIMULATION_IDENTIFIER;
extern std::string TOPOLOGY;
extern std::string TRAFFIC_PATTERN;
extern int INJECTION_RATE;
extern int MESSAGE_SIZE;
extern int MESSAGES_IN_SIMULATION;
extern int MESSAGES_IN_WARMUP;
extern int MESSAGES_IN_MEASUREMENT;
extern int SHORT_HOP_COST;
extern int LONG_HOP_COST;
extern int ROUTE_MODE;


#endif
