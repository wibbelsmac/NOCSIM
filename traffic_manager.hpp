#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include "message_queue.hpp"
#include "traffic_generator.hpp"
#include "traffic_analyzer.hpp"
#include "defines.hpp"

//#include "traffic_generator.hpp"

class TrafficManager {
    // Simulation Params
    int _message_count;
    int _added_message_count;
    int _removed_message_count;
    int _width;
    int _height;
		int _message_size;
		int _injection_period;
    int _update_count;
    int remove_count;
    //TrafficGenerator _traffic_generator;
    MessageQueue _message_queue;  
    TrafficGenerator* _traffic_generator; 
    TrafficAnalyzer _traffic_analyzer;    
  public:
    TrafficManager(int width, int height, int message_count);
    ~TrafficManager();
    void generate_messages(int time);
    bool pending_socket_message(int index, long long int time);
    NetworkFlit adding_message_to_network(int index, long long int time);
    void removing_message_from_network(int uid, int index, long long int time);    
    bool simulation_complete();
    void analyze_messages();
    void log_incomplete_messages();
    void print_queue_stats();
};
#endif
