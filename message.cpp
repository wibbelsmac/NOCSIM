#include "message.hpp"
#include "routing_table.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "defines.hpp"
#include <iomanip>

extern "C" {

Message::Message (uint32_t uid, uint32_t src, uint32_t dest, uint32_t message_size, uint32_t injection_rate, RoutingTable* rt) {
  _uid = uid;
	_source_index = src;
	_destination_index = dest;
  _address = rt->generate_route_addr32(src, dest, &_hop_count, ROUTE_MODE);
  _data = uid; // TODO Change to random value for when flitwidth is larger
  _message_size = message_size;
  _flit_vector = new FlitPtrVector(message_size); 
  _total_flit_transmission_time = 0;
  _total_flit_latency = 0;

  _first_receive_time = 0;


   _min_flit_latency = 0;
   _max_flit_latency = 0;
   _min_flit_transmission_time = 0;
   _max_flit_transmission_time = 0;

_first_enter_queue_time = 0;
_last_enter_queue_time = 0;
_first_leave_network_time = 0;
_last_leave_network_time = 0;
_first_enter_network_time = 0;
_last_enter_network_time = 0;
			
    
  // initialize vector
  for(uint i = 0; i < _flit_vector->size(); i++) {
    (*_flit_vector)[i] = new Flit();
  }
  _sent_flits = 0;
  _received_flits = 0;
	_injection_rate = injection_rate;
}


Message::Message (uint32_t uid, uint32_t src, uint32_t dest, uint32_t message_size, uint32_t injection_rate, uint32_t address, int hop_count) {
  _uid = uid;
	_source_index = src;
	_destination_index = dest;
  _address = address;
  _hop_count = hop_count;
  _data = uid; // TODO Change to random value for when flitwidth is larger
  _message_size = message_size;
  _flit_vector = new FlitPtrVector(message_size); 
  _total_flit_transmission_time = 0;
  _total_flit_latency = 0;

  _first_receive_time = 0;


   _min_flit_latency = 0;
   _max_flit_latency = 0;
   _min_flit_transmission_time = 0;
   _max_flit_transmission_time = 0;

_first_enter_queue_time = 0;
_last_enter_queue_time = 0;
_first_leave_network_time = 0;
_last_leave_network_time = 0;
_first_enter_network_time = 0;
_last_enter_network_time = 0;
			
    
  // initialize vector
  for(uint i = 0; i < _flit_vector->size(); i++) {
    (*_flit_vector)[i] = new Flit();
  }
  _sent_flits = 0;
  _received_flits = 0;
	_injection_rate = injection_rate;
}


Message::Message ()
{
 throw 20;
}

Message::~Message ()
{
  for(uint i = 0; i < _flit_vector->size(); i++) {
    delete (*_flit_vector)[i];
  }
  delete _flit_vector;
}

void Message::adding_message_to_queue(long long int time) { 
   _first_enter_queue_time = time;
   long long int enter_queue_time = time;
   _queued_flits = _flit_vector->size();
   for(uint i = 0; i < _flit_vector->size(); i++) {
    (*_flit_vector)[i]->enter_queue_time(enter_queue_time);
    enter_queue_time += _injection_rate;
  }
  _last_enter_queue_time = enter_queue_time;
}

void Message::generate_network_flit(NetworkFlit * NF) {
   NF->address = _address;
   NF->data = _uid;
}

void Message::print_message() {
 LOG_DEBUG << "Message uid: " << _uid << " Src: " << _source_index << " Dest: " << _destination_index << " Hop Count: " << _hop_count << std::setfill('0')  << " Addr: 0x" << std::hex << std::setw(8) << _address << std::setfill(' ')  << std::dec << " Message_size: " << _message_size;
 LOG_FILE << " Sent_Flits: " << _sent_flits << " Received Fits: " << _received_flits << std::endl; 
}

void Message::print_lat_info() {
  printf("Message FEQT: %lld LEQT: %lld FENT: %lld LENT: %lld FLNT: %lld LLNT: %lld\n", _first_enter_queue_time, _last_enter_queue_time,_first_enter_network_time, _last_enter_network_time, _first_leave_network_time, _last_leave_network_time);
}

bool Message::adding_flit_to_network(long long int ent, NetworkFlit* NF) {
	LOG_DEBUG << "Adding_Flit_to_net sent_flits: " << _sent_flits << "  MessageSize: " << _message_size << std::endl;  
  if(_sent_flits < _message_size) {
    (*_flit_vector)[_sent_flits]->enter_network_time(ent);
    if(_sent_flits == 0)
      _first_enter_network_time = ent;
    if(_sent_flits == (_message_size - 1)) { // if last flit
      _last_enter_network_time = ent;
    }
    _sent_flits++;
    generate_network_flit(NF);
    
    return true;
  }
  LOG_ERROR << "Message queue requesting more flits than the message size" << std::endl; 
  return false;
}

bool Message::removing_flit_from_network(long long int lnt) {
 // printf("Removing flit at time ll%d\n", lnt);
  if(_received_flits < _message_size) {
    (*_flit_vector)[_received_flits]->leave_network_time(lnt);
    (*_flit_vector)[_received_flits]->print_flit();
    // Transmission time analytics
    long long int transmission_time = (*_flit_vector)[_received_flits]->transmission_time();  
    long long int latency = (*_flit_vector)[_received_flits]->latency();  
    update_min_max_analytics(transmission_time, latency, _received_flits); 
    
    // message analytics
    if(_received_flits == 0) {
      _first_leave_network_time = lnt;
    }
    if(_received_flits == (_message_size-1)) {
      _last_leave_network_time = lnt;
    }
    overflow_accum_test(transmission_time, _total_flit_transmission_time); 
    _total_flit_transmission_time += transmission_time; 
    overflow_accum_test(latency, _total_flit_latency); 
    _total_flit_latency += latency; 
    _received_flits++;
  } else {
    _received_flits++;
    LOG_ERROR << "Error: too many received Flits expected: " << _message_size << " Received: " << _received_flits << std::endl;  
  }
  return (_received_flits == _message_size);
}

void Message::update_min_max_analytics(long long int transmission_time, long long int latency, uint32_t index) {
  if(index ==0) {
    _min_flit_latency = latency;  
    _max_flit_latency = latency;  
    _min_flit_transmission_time = transmission_time;  
    _max_flit_transmission_time = transmission_time;  
  } else {
    if(latency < _min_flit_latency) {
      _min_flit_latency = latency;
    } else if (latency > _max_flit_latency) {
      _max_flit_latency = latency;
    }
    if(transmission_time < _min_flit_transmission_time) {
      _min_flit_transmission_time = transmission_time;
    } else if (transmission_time > _max_flit_transmission_time) {
      _max_flit_transmission_time = transmission_time;
    }
  }
}



long long int Message::latency() {
  return _last_leave_network_time - _first_enter_queue_time;
}

bool Message::message_ready(long long int time) {
  if(_sent_flits < _message_size) {
    Flit* cpp_pos_compiler = (*_flit_vector)[_sent_flits];
    long long int next_queue_time = cpp_pos_compiler->enter_queue_time(); 

    if(next_queue_time <= time) {// if there are unsent flits and no messages from the "future" woooooo
      LOG_DEBUG << "Message available quetime " << next_queue_time << " < time " << time << std::endl;
      return true;
    } else {
     LOG_DEBUG << "NO Message available quetime " << next_queue_time << " < time " << time << "\n";
      return false;
    }
  }
  return false;
}

int32_t Message::total_hop_count() {
  return _hop_count * _received_flits;
}

int Message::unsent_messages() {
  return _message_size - _sent_flits;
}

long long int Message::last_enter_queue_time() {
  return _first_enter_queue_time + (_injection_rate * (_message_size -1));
}

long long int Message::transmission_time() {
  return _total_flit_transmission_time;
}

long long int Message::total_flit_latency() {
  return _total_flit_latency;
}
void Message::overflow_accum_test(long long int& a, long long int & result) {
  long long int ovf = a + result;
  if(ovf < 0)
    LOG_ERROR << "Message long long int overflow Detected" << std::endl;
}

bool Message::all_flits_received() {
  return ((int) (_received_flits >= _message_size));
}

double Message::offered_bandwidth() {
  double my_double(offered_interval());
  double mes_size = double(_message_size);
  return mes_size / my_double; 
}


double Message::min_flit_latency_per_hop() {
  double my_double(_min_flit_latency);
  double my_hop_count(_hop_count);
  printf("Message min_flit_latency %lld hop_count: %d min_flit_lat_per_hop: %f\n", _min_flit_latency, _hop_count, my_double / my_hop_count); 
  return my_double / my_hop_count;
}

double Message::max_flit_latency_per_hop() {
  double my_double(_max_flit_latency);
  double my_hop_count(_hop_count);
  printf("Message max_flit_latency %lld hop_count: %d max_flit_lat_per_hop: %f\n", _max_flit_latency, _hop_count, my_double / my_hop_count); 
  return my_double / my_hop_count;

}

double Message::offered_interval() {
  printf("Offered Interval %f",(1.0 * _last_enter_queue_time) - (1.0 * _first_enter_queue_time)); 
  return (1.0 * _last_enter_queue_time) - (1.0 * _first_enter_queue_time);
}

double Message::actual_bandwidth() {
  double my_double(actual_interval());
  double mes_size = double(_message_size);
  return mes_size / my_double; 
}

double Message::actual_interval() {
 printf("Actual Interval %f",(1.0 * _last_leave_network_time) - (1.0 *_first_leave_network_time)); 
 return (1.0 * _last_leave_network_time) - (1.0 *_first_leave_network_time);
}

}
