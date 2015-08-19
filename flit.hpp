#ifndef FLIT_H
#define FLIT_H

#include <stdint.h>
#include "routing_table.hpp"
#include <vector>



class Flit {
    
    long long int _enter_queue_time;
    long long int _enter_network_time;
    long long int _leave_network_time;
    
  public:
    Flit();
    inline long long int enter_queue_time() { return _enter_queue_time; };
    inline void enter_queue_time(long long int qt) {_enter_queue_time = qt; };
    inline void enter_network_time(long long int ent) {_enter_network_time = ent; };
    inline long long int enter_network_time() { return _enter_network_time; };
    inline long long int leave_network_time() { return _leave_network_time; };
    inline void leave_network_time(long long int lnt) {_leave_network_time = lnt; };
    long long int latency();
    long long int transmission_time();
    void print_flit();
    
};
#endif

