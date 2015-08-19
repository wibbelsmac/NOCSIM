#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <vector>
#include "message.hpp"
#include "routing_table.hpp"
#include "message_queue.hpp"

//#include "traffic_generator.hpp"

class TrafficGenerator {
    //TrafficGenerator _traffic_generator;
    uint32_t _uid;
    uint32_t _node_count;
    uint32_t get_uid(); // Doesnt check for overflow but hopefully we send less than 2^32 messages
    uint32_t rand_range(uint32_t min, uint32_t max);
    RoutingTable * _routing_table;
		uint32_t _injection_period;
		int _message_size;
    int _generated_messages;
    int _height;
    int _width;
  public:
    TrafficGenerator();
    TrafficGenerator(int width, int height, int message_size, uint32_t injection_period);
    void test_some_cores(long long int time, MessageQueue* message_queue);
    void generate_messages(long long int time, MessageQueue* message_queue);
    void generate_non_row_column_conflict_messages(long long int time, MessageQueue* message_queue);
    void generate_neighbor_messages(long long int time, MessageQueue* message_queue); 
    void generate_one_right_message(long long int time, MessageQueue* message_queue); 
    void conflict_test(long long int time, MessageQueue* message_queue); 
    void conflict_test1(long long int time, MessageQueue* message_queue); 
    inline void node_count(uint32_t node_count) {_node_count = node_count;};
    inline int generated_messages() { return _generated_messages;};
};
#endif
