#include "traffic_generator.hpp"
#include "message.hpp"
#include <stdio.h>
#include <stdint.h>
#include <cstdlib>
#include "defines.hpp"
#include <time.h>       /* time */
#include <math.h>
#include <stack>


extern "C" {


TrafficGenerator::TrafficGenerator(int width, int height, uint32_t injection_rate, 
                                   double time_units_per_sec, double time_units_per_generation_period,
                                   int message_gen_in_warm, int message_gen_in_measurement,
                                   int message_gen_in_cooldown) {
  _uid = 0;
  _node_count = width * height;
  _routing_table = new RoutingTable(width, height, SHORT_HOP_COST, LONG_HOP_COST);
 	_injection_rate = injection_rate;
  _generated_messages = 0;
  _generated_flits = 0;
  _width = width;
  srand (time(NULL));
  _height = height;
  _time_units_per_sec = time_units_per_sec;
  _time_units_per_generation_period = time_units_per_generation_period;
  _message_gen_in_warmup = message_gen_in_warm;
  _message_gen_in_measurement = message_gen_in_measurement;
  _message_gen_in_cooldown = message_gen_in_cooldown;
  _message_generation_intervals = 0;
  _message_gen_in_simulation = _message_gen_in_warmup + _message_gen_in_measurement + _message_gen_in_cooldown;

}

int TrafficGenerator::created_messages() {
  return _last_uid_in_simulation;
}


int TrafficGenerator::messages_in_warmup() {
  return _last_uid_in_warmup;
}

int TrafficGenerator::messages_in_measurement() {
  return _last_uid_in_measurement - _last_uid_in_warmup;
}

TrafficGenerator::TrafficGenerator() {
  _uid = 0;
}


void TrafficGenerator::update_message_generation_intervals() {
  _message_generation_intervals++;
  if(_message_generation_intervals == _message_gen_in_warmup){
    _last_uid_in_warmup = _uid; 
  } else if(_message_generation_intervals == (_message_gen_in_warmup + _message_gen_in_measurement)){
    _last_uid_in_measurement = _uid;
  } else if(_message_generation_intervals == (_message_gen_in_warmup + _message_gen_in_measurement + _message_gen_in_cooldown)) {
    _last_uid_in_simulation = _uid;
  }
}


bool TrafficGenerator::more_generation_intervals() {
  return (_message_generation_intervals < _message_gen_in_simulation);
}

void TrafficGenerator::generate_non_row_column_conflict_messages(long long int time, MessageQueue* message_queue) {
	int message_size = 10;
  if(!more_generation_intervals()) 
      return;
  //printf("Generatubg Message\n");      
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
	for (int i=0; i < num_of_transmitting_cores; i++)
		{
			uint32_t src_x   = rand_range(0, _width-1);
			uint32_t src_y   = rand_range(0, _height-1);
			uint32_t dest_x  = rand_range(0, _width-2);
			uint32_t dest_y  = rand_range(0, _height-2);
      if(dest_x >= src_x)
        dest_x++;
      if(dest_y >= src_y)
        dest_y++;
      uint32_t src = src_x * _height + src_y;
      uint32_t dest = dest_x * _height + dest_y;
			uint32_t uid = get_uid();
			LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
			Message* msg = new Message(uid, src, dest, message_size, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
	    _generated_flits+= message_size;

    }
		
  update_message_generation_intervals();  	
}

void TrafficGenerator::generate_messages(long long int time, MessageQueue* message_queue) {
  if(!more_generation_intervals()) {
    return;
  }
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
  long long int queue_time = time; 
	int message_size = 10;
  double injection_rate_d = _injection_rate * pow(2,20);
  int num_messages = (injection_rate_d  * _time_units_per_generation_period)/ (_time_units_per_sec * message_size);
  
  long long int time_step = _time_units_per_generation_period  / num_messages;
  int j =0;
  while(j < num_messages) {
    for (int i=0; i < num_of_transmitting_cores; i++)
		{
			uint32_t src   = rand_range(0, _node_count-1);
			uint32_t dest  = rand_range(0, _node_count-1);
			uint32_t uid = get_uid();
			while (src == dest) {
        dest = rand_range(0, _node_count-1);  
      }
			LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
			Message* msg = new Message(uid, src, dest, message_size, _routing_table);
		  message_queue->add_message(src, msg, queue_time);
      _generated_messages++;
     	_generated_flits+= message_size;
      j++;
      queue_time += time_step;
      if(j == num_messages) 
        break;
    }
  }	
  update_message_generation_intervals();  	
}

void TrafficGenerator::generate_bursty_traffic(long long int time, MessageQueue* message_queue) {
  if(!more_generation_intervals()) {
    return;
  }
	printf("Generatubg Message\n");      
  long long int queue_time = time; 
  double injection_rate_d = _injection_rate * pow(2,20);
  int n = pow(2,AGGREGATION_LEVEL);
  double time_step = double(_time_units_per_generation_period)  / double(n);
  double message_volume = (injection_rate_d  * _time_units_per_generation_period)/ (_time_units_per_sec); 
  double number_of_flits_per_transmission = message_volume / double(BYTES_PER_FLIT);
  
    for (int i=0; i < _node_count; i++)
		{
      int b_model_message_index = 0;
      std::stack< BModelPair > message_stack;
      int N = pow(2,AGGREGATION_LEVEL);
      message_stack.push(std::make_pair(0,number_of_flits_per_transmission));
      while(!message_stack.empty()) {
        printf("Generating message %d/%d",b_model_message_index,N);
        BModelPair b_model_pair = message_stack.top();
        int k = b_model_pair.first;
        double v = b_model_pair.second;
        message_stack.pop();
        if(k == AGGREGATION_LEVEL) { // send message
			    uint32_t src = i;
          uint32_t dest  = rand_range(0, _node_count-1);
          uint32_t uid = get_uid();
          int message_size = v;
          long long int queue_time = double(b_model_message_index) * time_step + time;  
          while (src == dest) {
            dest = rand_range(0, _node_count-1);  
          }
          LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
          if(v >0) {
            Message* msg = new Message(uid, src, dest, message_size, _routing_table);
            message_queue->add_message(src, msg, queue_time);
            _generated_messages++;
            _generated_flits+= message_size;
          }
          b_model_message_index++;
        } else {
          int heads_tails = rand_range(0,1);
          double remaining_volume_fraction = v * (1.0 - BIAS_PARAM);
          int remaining_volume = round(remaining_volume_fraction);
          int majority_voume = v - remaining_volume;
          
          if(heads_tails == 0) {
            message_stack.push(std::make_pair(k+1, majority_voume));
            message_stack.push(std::make_pair(k+1, remaining_volume));
          } else if(heads_tails == 1) {
            message_stack.push(std::make_pair(k+1, remaining_volume));
            message_stack.push(std::make_pair(k+1, majority_voume));
          }
        }

      }
    }
  update_message_generation_intervals();  	
}


void TrafficGenerator::generate_neighbor_messages(long long int time, MessageQueue* message_queue) {
  if(!more_generation_intervals()) 
      return;
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
	//printf("Generatubg Message\n"); 
  long long int queue_time = time; 
	int message_size = 10;
  int num_messages = (_injection_rate  * _time_units_per_generation_period)/ (_time_units_per_sec * message_size);
  long long int time_step = _time_units_per_generation_period  / num_messages;
  int j =0;
  while(j < num_messages) {
    for (int i=0; i < num_of_transmitting_cores; i++)
		{
			uint32_t src   = rand_range(0, _node_count-1);
			uint32_t dest  = (src + _height) % (_width * _height);
			uint32_t uid = get_uid();
			LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
			Message* msg = new Message(uid, src, dest, message_size, _routing_table);
		  message_queue->add_message(src, msg, queue_time);
      _generated_messages++;
    	_generated_flits+= message_size;
      j++;
      queue_time += time_step;
      if(j == num_messages) 
        break;
    }
  }	
  update_message_generation_intervals();  	
	
}
/*
void TrafficGenerator::test_some_cores(long long int time, MessageQueue* message_queue) {
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
	//printf("Generatubg Message\n");      
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  if(num_of_transmitting_cores > remaining_messages)
    num_of_transmitting_cores = remaining_messages;
  int transmitting = 1;
	for (int i=0; i < transmitting; i++)
		{
			uint32_t src   = rand_range(0, _node_count-1);
			uint32_t dest  = (src + _height) % (_width * _height);
			uint32_t uid = get_uid();
			LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
			Message* msg = new Message(uid, src, dest, message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
    	_generated_flits+= message_size;

    }
}

void TrafficGenerator::conflict_test(long long int time, MessageQueue* message_queue) {
	//printf("Generatubg Message\n");      
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  uint32_t src1 = 0;
  uint32_t src2 = 9;
  uint32_t dest  = 8;
			uint32_t uid1 = get_uid();
			uint32_t uid2 = get_uid();
			LOG_DEBUG << "Adding Message: uid: " << uid1 << " source: " << src1 <<" dest: " << dest << std::endl;      
			LOG_DEBUG << "Adding Message: uid: " << uid2 << " source: " << src2 <<" dest: " << dest << std::endl;      
			Message* msg1 = new Message(uid1, src1, dest, message_size, _injection_period, _routing_table);
			Message* msg2 = new Message(uid2, src2, dest, message_size, _injection_period, _routing_table);
		  message_queue->add_message(src1, msg1, time);
		  message_queue->add_message(src2, msg2, time);
      _generated_messages += 2;
      _generated_flits += message_size;
}

void TrafficGenerator::conflict_test1(long long int time, MessageQueue* message_queue) {
	//printf("Generatubg Message\n");      
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  uint32_t src_arr [8] =  { 59, 51, 43, 35, 27, 19, 11, 3};
  uint32_t dest_arr [8] = { 35, 27, 19, 11, 3, 59, 51, 43};

 for(int i = 0; i < 7; i++) {
		  uint32_t uid = get_uid(); 
      LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], message_size, _injection_period, _routing_table);
      message_queue->add_message(src_arr[i], msg1, time);
      _generated_messages ++;
	    _generated_flits += message_size;
 }
}
void TrafficGenerator::generate_one_right_message(long long int time, MessageQueue* message_queue) {
			uint32_t src   = 0;
			uint32_t dest  = 63-8;
			uint32_t uid = get_uid();
      LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
	Message* msg = new Message(uid, src, dest, message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
	    _generated_flits += message_size;

}
*/





uint32_t TrafficGenerator::get_uid() {
  _uid++;
  return _uid;
}

uint32_t TrafficGenerator::rand_range(uint32_t min, uint32_t max)
{
	double scaled = (double)rand()/RAND_MAX;
	return (max - min + 1)*scaled +min;
}
}
