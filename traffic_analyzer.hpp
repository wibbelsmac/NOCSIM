#ifndef TRAFFIC_ANALYZER_H
#define TRAFFIC_ANALYZER_H

#include <vector>
#include "message.hpp"
#include "routing_table.hpp"
#include "message_queue.hpp"
#include <string>

//#include "traffic_generator.hpp"

class TrafficAnalyzer {
    //TrafficAnalyzer _traffic_generator;

  std::string filename;
  public:
    TrafficAnalyzer();
    void add_message_vector_to_queue(std::vector< Message *>* msg_vect);
    void add_flit_to_network(uint32_t uid, uint64_t time); 
    void remove_flit_from_network(uint32_t uid, uint64_t time);
    void analyze_message_queue(MessageQueue* message_queue, uint32_t injection_ratei, uint32_t min_uid, uint32_t max_uid, int total_flits);
    void init_results_file(); 
};
#endif
