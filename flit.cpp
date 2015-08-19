#include "flit.hpp"
#include "routing_table.hpp"
#include "defines.hpp"

extern "C" {

Flit::Flit ()
{
 _enter_network_time = 0;
 _leave_network_time = 0;
 _enter_queue_time = 0; 
}

long long int Flit::latency() {
  if(_leave_network_time < _enter_queue_time) 
    LOG_ERROR << "Error Flit Left Network Before Entering Queue Enter: " << _enter_queue_time << " Leave: " << _leave_network_time << std::endl; 
  return _leave_network_time - _enter_queue_time;
}

long long int Flit::transmission_time() {
  if(_leave_network_time < _enter_network_time) 
    LOG_ERROR << "Error Flit Left Network Before Entering Network Enter: " << _enter_network_time << " Leave: " << _leave_network_time << std::endl; 
  return _leave_network_time - _enter_network_time;
}

void Flit::print_flit() {
 printf("FLIT: EQT: %lld ENT: %lld LNT: %lld LAT: %lld\n",_enter_network_time, _enter_network_time, _leave_network_time, latency()); 
}
}
