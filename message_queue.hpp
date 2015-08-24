#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <vector>
#include <queue>
#include <map>
#include "message.hpp"

extern "C" {

typedef std::map <uint32_t, Message*> MessageMap;
typedef MessageMap::iterator MessageMapIter;

class MessageQueue {

    std::vector<std::queue<Message*> > _message_queues;
    typedef std::map <uint32_t, Message*> MessageMap;
    typedef MessageMap::iterator MessageMapIterator;
    MessageMap _completed_message_queue;

    uint32_t _completed_messages;
    uint32_t _queued_messages;  
    
    uint32_t _queued_flits;
    uint32_t _transit_flits;
    uint32_t _received_flits;

  public:
    MessageQueue(int width, int height);
    MessageQueue();
    void add_message(int index, Message* message, long long int time);
    void add_messages(std::vector< Message* >* message_vect); 
    NetworkFlit add_flit_to_network(int index, long long int time);
    void remove_flit_from_network(uint32_t uid, uint32_t index, long long int time);
    bool pending_messages(int index, long long int time);
    void init(int width, int height);
    bool received_all_messages();
    void print_queue_stats();
    void print_queue_stats_to_term();
    MessageMap* gimme_the_message_map(); // not good practice but the deadline draws near
    void log_incomplete_messages();
  
};


}

#endif
