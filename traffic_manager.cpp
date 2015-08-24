#include "traffic_manager.hpp"

#include <stdio.h>

extern "C" {

TrafficManager::TrafficManager(int width, int height, int message_gen_in_warmup, int message_gen_in_measurement,
                               int message_gen_in_cooldown, int injection_rate, double time_units_per_sec,
                              double time_units_per_generation_period) {
 
  _message_gen_in_warmup = message_gen_in_warmup;
  _message_gen_in_measurement = message_gen_in_measurement;
  _message_gen_in_cooldown = message_gen_in_cooldown;
  _added_message_count = 0;
  _removed_message_count = 0;
 	_injection_rate = injection_rate;
  _width = width;
  _height = height;
  _message_queue.init(width, height);
  _traffic_generator = new TrafficGenerator(width, height, _injection_rate, time_units_per_sec, time_units_per_generation_period,
                                            _message_gen_in_warmup, _message_gen_in_measurement, _message_gen_in_cooldown);
  _update_count = 0;
  remove_count = 0;
}

TrafficManager::~TrafficManager() {
  delete _traffic_generator;

}


void TrafficManager::generate_messages(int time) {	
  
  //LOG_DEBUG << "Running Generate Messages method in traffic Manager\n");
    if(TRAFFIC_PATTERN.compare("random") == 0) {
      _traffic_generator->generate_messages(time, &_message_queue);
   // }else if(TRAFFIC_PATTERN.compare("neighbor") == 0) {
   //   _traffic_generator->generate_neighbor_messages(time, &_message_queue);
   // }else if(TRAFFIC_PATTERN.compare("test") == 0) {
   //   _traffic_generator->test_some_cores(time, &_message_queue);
   // }else if(TRAFFIC_PATTERN.compare("test1") == 0) {
   //   _traffic_generator->generate_one_right_message(time, &_message_queue);
   // }else if(TRAFFIC_PATTERN.compare("conflict") == 0) {
   //   _traffic_generator->conflict_test(time, &_message_queue);
   //  }else if(TRAFFIC_PATTERN.compare("conflict1") == 0) {
   //   _traffic_generator->conflict_test1(time, &_message_queue);
   //  }else if(TRAFFIC_PATTERN.compare("diff_row_column") == 0) {
   //   _traffic_generator->generate_non_row_column_conflict_messages(time, &_message_queue);
    } else {
      LOG_ERROR << "NO Recognized Traffic Pattern SPecified" << std::endl;
    }
  }

NetworkFlit TrafficManager::adding_message_to_network (int index, long long int time) {
  _added_message_count++;
  LOG_DEBUG << "Add Message count " << _added_message_count << "\n";
 return _message_queue.add_flit_to_network(index, time);
 // add traffic analyzer code;
  
}

void TrafficManager::analyze_messages() {
  int messages_in_warmup = _traffic_generator->messages_in_warmup();
  int messages_in_warm_and_meas = messages_in_warmup + _traffic_generator->messages_in_measurement();
  _traffic_analyzer.analyze_message_queue(&_message_queue, _injection_rate, messages_in_warmup, messages_in_warm_and_meas, _traffic_generator->generated_flits()); 
  //_traffic_analyzer.analyze_message_queue(&_message_queue, _injection_period, 0, _message_count); 
}


void TrafficManager::log_incomplete_messages () {
  _message_queue.log_incomplete_messages();
}

void TrafficManager::removing_message_from_network (int uid, int index, long long int time) {
  remove_count++;
  LOG_DEBUG << "Remove count " << remove_count << "\n";
  _message_queue.remove_flit_from_network(uid, index, time);
}

bool TrafficManager::pending_socket_message (int index, long long int time) { 
  return	_message_queue.pending_messages(index, time);  
}

bool TrafficManager::simulation_complete() {
  if(_update_count % 500 == 0)
    _message_queue.print_queue_stats();
  _update_count++;
  if(_traffic_generator->more_generation_intervals()) {
    return false;
  } else {
    return _message_queue.received_all_messages();
  }
}

void TrafficManager::print_queue_stats() {
  _message_queue.print_queue_stats_to_term();
}
  

}
