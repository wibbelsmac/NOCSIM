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
		uint32_t _injection_rate;
    int _generated_messages;
    int _generated_flits;
    int _height;
    int _width;
    int _last_uid_in_warmup, _last_uid_in_measurement, _last_uid_in_simulation;
    int _message_gen_in_warmup, _message_gen_in_measurement, _message_gen_in_cooldown, _message_gen_in_simulation;
    double _time_units_per_sec, _time_units_per_generation_period;
    int _message_generation_intervals;
    void update_message_generation_intervals();

  public:

    TrafficGenerator();
    TrafficGenerator(int width, int height, uint32_t injection_rate, 
                     double time_units_per_sec, double time_units_per_generation_period,
                     int message_gen_in_warm, int message_gen_in_measurement,
                     int message_gen_in_cooldown);
    bool more_generation_intervals();
    void test_some_cores(long long int time, MessageQueue* message_queue);
    void generate_messages(long long int time, MessageQueue* message_queue);
    void generate_non_row_column_conflict_messages(long long int time, MessageQueue* message_queue);
    void generate_neighbor_messages(long long int time, MessageQueue* message_queue); 
    void generate_one_right_message(long long int time, MessageQueue* message_queue); 
    void conflict_test(long long int time, MessageQueue* message_queue); 
    void conflict_test1(long long int time, MessageQueue* message_queue); 
    inline void node_count(uint32_t node_count) {_node_count = node_count;};
    inline int generated_messages() { return _generated_messages;};
    inline int generated_flits() {return _generated_flits;};
    int created_messages();
    int messages_in_warmup();
    int messages_in_measurement();
};
#endif
