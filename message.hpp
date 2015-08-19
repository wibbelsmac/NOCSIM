#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <vector>
#include <stdio.h>
#include "routing_table.hpp"
#include "flit.hpp"

struct NetworkFlit {
  uint32_t address;
  uint32_t data;
};

typedef std::vector< NetworkFlit > NetworkFlitVector;

typedef std::vector < Flit* > FlitPtrVector;
class Message {
    
		int32_t _injection_rate;
    int32_t _message_size; // in terms of flits
    long long int _first_enter_queue_time;
    long long int _last_enter_queue_time;
    long long int _first_leave_network_time;
    long long int _last_leave_network_time;
    long long int _first_enter_network_time;
    long long int _last_enter_network_time;
			    

    uint32_t _source_index;
    uint32_t _destination_index;
    uint32_t _hop_count;
		
    uint32_t _uid; 
    uint32_t _address;
    uint32_t _data;   

    int32_t _queued_flits;
    int32_t _sent_flits;
    int32_t _received_flits;

    long long int _total_flit_transmission_time;
    long long int _total_flit_latency;

    long long int _first_receive_time;


    long long int _min_flit_latency;
    long long int _max_flit_latency;
    long long int _min_flit_transmission_time;
    long long int _max_flit_transmission_time;

    FlitPtrVector* _flit_vector;
	
    void update_min_max_analytics(long long int transmission_time, long long int latenc, uint32_t index);
    void overflow_accum_test(long long int& a, long long int& result);
    void generate_network_flit(NetworkFlit * NF ); 
    
  public:
    Message(uint32_t uid, uint32_t src, uint32_t dest, uint32_t message_size, uint32_t injection_rate, RoutingTable* rt);
    Message(uint32_t uid, uint32_t src, uint32_t dest, uint32_t message_size, uint32_t injection_rate, uint32_t address, int hop_count);
    Message();
    ~Message();
    inline uint32_t uid() {return _uid; };
    inline uint32_t source_index() { return _source_index; };
    inline void source_index(uint32_t qt) {_source_index = qt; };
		inline uint32_t dest_index() { return _destination_index; };
    inline void dest_index(uint32_t qt) {_destination_index = qt; };
void adding_message_to_queue(long long int time);
    bool adding_flit_to_network(long long int ent, NetworkFlit* NF);
    bool removing_flit_from_network(long long int lnt);
    bool message_ready(long long int time);
    inline uint32_t address() { return _address; };
    inline void address(uint32_t add) {_address = add; };
    inline uint32_t data() { return _data; };
    inline void data(uint32_t dat) {_data = dat; };
    
    NetworkFlitVector* return_network_flit(long long int time, uint32_t injection_rate);
    long long int latency();
    long long int last_enter_queue_time();
    long long int transmission_time();
    long long int total_flit_latency();
    int32_t total_hop_count();
    bool all_flits_received();
    double offered_interval();
    double actual_interval();
    double offered_bandwidth();
    double actual_bandwidth();
    double min_flit_latency_per_hop();
    double max_flit_latency_per_hop();
    int unsent_messages();
    inline int sent_flits () {return _sent_flits;};
    inline int message_size () {return _message_size; };
    inline int received_flits () {return _received_flits;};
    void print_message();
    void print_lat_info();
};
#endif

