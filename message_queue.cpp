#include "message_queue.hpp"
#include "defines.hpp"

extern "C" {

MessageQueue::MessageQueue (int width, int height) {
  init(width, height);
}


void MessageQueue::init (int width, int height) {
 _message_queues.resize(width * height);
 _completed_messages = 0;
 _queued_messages = 0;
 _transit_flits =0;
 _received_flits = 0;
 _queued_flits = 0;
 //for (int i = 0; i <(width * height); i++) {
 // _message_queues[i].
 //} 
}

MessageQueue::MessageQueue () {}

void MessageQueue::add_message(int index, Message* msg, long long int time) {
  if(!_message_queues[index].empty()) {
    long long int last_flit_time =  _message_queues[index].back()->last_enter_queue_time();
    long long int actual_enter_time = (time > last_flit_time) ? time : last_flit_time;
    _queued_flits += msg->message_size();
    msg->adding_message_to_queue(actual_enter_time);
  } else {
    _queued_flits += msg->message_size();
    msg->adding_message_to_queue(time);
  }
  _message_queues[index].push(msg);
  _completed_message_queue[msg->uid()] = msg;
  _queued_messages++;
}

/*
void MessageQueue::add_messages(std::vector< Message* >* message_vect) {
	printf("UNIMPLEMENTED!!!");  
	 for (std::vector< Message* >::iterator it = message_vect->begin() ; it != message_vect->end(); it++) {
     uint32_t index = (*it)->source_index();
     _message_queues[index].push(*it); 
     _queued_messages++;
     _queued_flits += MESSAGE_SIZE;
   }

}
*/
 

NetworkFlit MessageQueue::add_flit_to_network(int index, long long int time) {
  NetworkFlit NF;
  if((index <  _message_queues.size()) && !_message_queues[index].empty()) {
    Message * message_temp_ptr = _message_queues[index].front();
    if(message_temp_ptr->adding_flit_to_network(time, &NF)) {
	   LOG_DEBUG << "Adding flit "  << message_temp_ptr->sent_flits() << " of " <<  message_temp_ptr->message_size() << " at index " << index << " with uid: " << message_temp_ptr->uid() << " at time: " << time << "\n";
      _transit_flits++;
      if(message_temp_ptr->unsent_messages() == 0) {
        _message_queues[index].pop();
      }
     return NF;
    }
  } else {
    LOG_ERROR << "Attempting to Flit of index that is empty or doesnt exist index: " << index << std::endl;
  }
}

void MessageQueue::remove_flit_from_network(uint32_t uid, uint32_t index, long long int time) {
    printf("top of remove flit messagequeue with uid %d at time: %lld\n", uid, time);
    if(_completed_message_queue.find(uid) != _completed_message_queue.end()) {
    //printf("found message %d\n", uid);
      _received_flits++;
      Message* msg =_completed_message_queue[uid];
      if(msg->dest_index() != index) {
        std::cout << "ERROR Received flit from wrong destination! Expecting index: " << msg->dest_index() << " Received index: " << index << std::endl;
        LOG_ERROR << "Received flit from wrong destination! Expecting index: " << msg->dest_index() << " Received index: " << index << std::endl;
      } else {
        std::cout << "Received flit from correct index expected:" << msg->dest_index() << " Received: " << index << std::endl;
      }
      if(msg->removing_flit_from_network(time)) {
        printf("msg removed with uid %d and latency %lld\n", uid, msg->latency());
       _completed_messages++;
      } 
	    LOG_DEBUG << "Receiving flit " <<  msg->received_flits() << " of " <<  msg->message_size() << " with uid " << uid << " from network\n";

    } else { 
     LOG_ERROR << "Cannot Find Message for uid " << uid << " map size:" << _completed_message_queue.size() << "\n";
    }
}

bool MessageQueue::pending_messages(int index, long long int time) {
	if(!_message_queues[index].empty()) {  
			//printf("Message queue is not empty\n");		
			if(_message_queues[index].front()->message_ready(time)) {
        return true;
      }
		}
	//printf("No Messages in  queue\n");  
	return false;
}

void MessageQueue::print_queue_stats() {
  LOG_DEBUG << "Queued Flits: " << _queued_flits << " Transit Flits: " << _transit_flits << " Received Flits: " << _received_flits << "\n";
}

void MessageQueue::print_queue_stats_to_term() {
  std::cout << "Queued Flits: " << _queued_flits << " Transit Flits: " << _transit_flits << " Received Flits: " << _received_flits << "\n";
}

bool MessageQueue::received_all_messages() {
  if(_completed_messages > _queued_messages) {
    LOG_DEBUG << "More completed messages that queued messages\n";
  }
  
  return (_completed_messages >= _queued_messages) && (_queued_messages != 0);
}

MessageMap* MessageQueue::gimme_the_message_map() {
	return &_completed_message_queue;	 
}


void MessageQueue::log_incomplete_messages() {
 for(MessageMapIterator iter= _completed_message_queue.begin(); iter != _completed_message_queue.end(); ++iter) {
  if(!(iter->second->all_flits_received())){
   LOG_DEBUG << "Error Incomplete ";
   iter->second->print_message();  
  } 
 } 
}

}
