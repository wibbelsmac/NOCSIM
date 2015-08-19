#include "traffic_generator.hpp"
#include "message.hpp"
#include <stdio.h>
#include <stdint.h>
#include <cstdlib>
#include "defines.hpp"
#include <time.h>       /* time */

extern "C" {


TrafficGenerator::TrafficGenerator(int width, int height, int message_size, uint32_t injection_period) {
  _uid = 0;
  _node_count = width * height;
  _routing_table = new RoutingTable(width, height, SHORT_HOP_COST, LONG_HOP_COST);
	_message_size = message_size;
 	_injection_period = injection_period;
  _generated_messages = 0;
  _width = width;
  srand (time(NULL));
  _height = height;
}

TrafficGenerator::TrafficGenerator() {
  _uid = 0;
}

void TrafficGenerator::generate_non_row_column_conflict_messages(long long int time, MessageQueue* message_queue) {
	//printf("Generatubg Message\n");      
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  if(num_of_transmitting_cores > remaining_messages)
    num_of_transmitting_cores = remaining_messages;
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
			Message* msg = new Message(uid, src, dest, _message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
    }
		
	
}void TrafficGenerator::generate_messages(long long int time, MessageQueue* message_queue) {
	//printf("Generatubg Message\n");      
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  if(num_of_transmitting_cores > remaining_messages)
    num_of_transmitting_cores = remaining_messages;
	for (int i=0; i < num_of_transmitting_cores; i++)
		{
			uint32_t src   = rand_range(0, _node_count-1);
			uint32_t dest  = rand_range(0, _node_count-1);
			uint32_t uid = get_uid();
			while (src == dest) {
        dest = rand_range(0, _node_count-1);  
      }
			LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
			Message* msg = new Message(uid, src, dest, _message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
    }
		
	
}

void TrafficGenerator::generate_neighbor_messages(long long int time, MessageQueue* message_queue) {
  int num_of_transmitting_cores = rand_range(0, (unsigned int)_node_count-1);
	//printf("Generatubg Message\n"); 
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  if(num_of_transmitting_cores > remaining_messages)
    num_of_transmitting_cores = remaining_messages;

	for (int i=0; i < num_of_transmitting_cores; i++)
		{
			uint32_t src   = rand_range(0, _node_count-1);
			uint32_t dest  = (src + _height) % (_width * _height);
			uint32_t uid = get_uid();
			LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
			Message* msg = new Message(uid, src, dest, _message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
    }
}

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
			Message* msg = new Message(uid, src, dest, _message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
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
			Message* msg1 = new Message(uid1, src1, dest, _message_size, _injection_period, _routing_table);
			Message* msg2 = new Message(uid2, src2, dest, _message_size, _injection_period, _routing_table);
		  message_queue->add_message(src1, msg1, time);
		  message_queue->add_message(src2, msg2, time);
      _generated_messages += 2;
}

void TrafficGenerator::conflict_test1(long long int time, MessageQueue* message_queue) {
	//printf("Generatubg Message\n");      
  int remaining_messages = MESSAGES_IN_SIMULATION - _generated_messages;     
  uint32_t src_arr [8] =  { 59, 51, 43, 35, 27, 19, 11, 3};
  uint32_t dest_arr [8] = { 35, 27, 19, 11, 3, 59, 51, 43};

 for(int i = 0; i < 7; i++) {
		  uint32_t uid = get_uid(); 
      LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], _message_size, _injection_period, _routing_table);
      message_queue->add_message(src_arr[i], msg1, time);
      _generated_messages ++;
 }
}
void TrafficGenerator::generate_one_right_message(long long int time, MessageQueue* message_queue) {
			uint32_t src   = 0;
			uint32_t dest  = 63-8;
			uint32_t uid = get_uid();
      LOG_DEBUG << "Adding Message: uid: " << uid << " source: " << src <<" dest: " << dest << std::endl;      
	Message* msg = new Message(uid, src, dest, _message_size, _injection_period, _routing_table);
		  message_queue->add_message(src, msg, time);
      _generated_messages++;
}

uint32_t TrafficGenerator::get_uid() {
  int temp = _uid;
  _uid++;
  return _uid;
}





uint32_t TrafficGenerator::rand_range(uint32_t min, uint32_t max)
{
	double scaled = (double)rand()/RAND_MAX;
	return (max - min + 1)*scaled +min;
}}
