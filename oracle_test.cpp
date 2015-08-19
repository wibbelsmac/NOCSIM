#include "defines.hpp"
#include "oracle.hpp"
#include "message.hpp"
#include <vector>
#include "oracle_test.hpp"

std::ofstream ERROR_FILE;
std::ofstream LOG_FILE;
std::ofstream RESULTS_FILE;

std::string ERROR_FILENAME = "oracle_test.err";
std::string LOG_FILENAME = "oracle_test.log";
std::string RESULTS_FILENAME = "oracle_test.results";
std::string SIMULATION_IDENTIFIER = "oracle_test";
std::string TOPOLOGY = "taurus";
std::string TRAFFIC_PATTERN = "random";
int INJECTION_RATE = 10;
int MESSAGE_SIZE = 10;
int MESSAGES_IN_SIMULATION = 4;
int MESSAGES_IN_WARMUP = 1;
int MESSAGES_IN_MEASUREMENT = 2;
int SHORT_HOP_COST = 1;
int LONG_HOP_COST = 1;
int ROUTE_MODE = 1;



int get_uid() {
  static int uid = 0;
  return ++uid;
}
bool run_simple_horiz_cycle_test() {
 std::vector<Message*> my_vector;
 Oracle test_oracle;
 uint32_t src_arr [4] =  { 3, 19, 35, 51};
 uint32_t dest_arr [4] = { 27, 43, 59, 11};
 uint32_t addr = 0x03000000;
  for(int i = 0; i < 4; i++) {
		  uint32_t uid = get_uid(); 
//      std::cout << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], MESSAGE_SIZE, 1, addr, 3);
      my_vector.push_back(msg1);
      if(test_oracle.test_message_for_cycle(msg1)) {
        std::cout << "Loop Detected while adding message" << std::endl;
        std::cout << "Removing first message" << std::endl;
        test_oracle.remove_message(my_vector[0]);
        if(!test_oracle.test_message_for_cycle(msg1)) {
          std::cout << "Sucuess loop removed" << std::endl;
        } else {
          std::cout << "Error loop still exists!" << std::endl;
        }
      } else {
        std::cout << "Sucuesfully added message" << std::endl;
        test_oracle.add_message(msg1);
      }     
  }
  return true;
}

void run_horiz_cycle_test() {
 std::vector<Message*> my_vector;
 Oracle test_oracle;
 uint32_t src_arr [8] =  { 59, 51, 43, 35, 27, 19, 11, 3};
 uint32_t dest_arr [8] = { 35, 27, 19, 11, 3, 59, 51, 43};
 uint32_t addr = 0xFC000000;
   for(int i = 0; i < 7; i++) {
		  uint32_t uid = get_uid(); 
      std::cout << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], MESSAGE_SIZE, 1, addr, 3);
      my_vector.push_back(msg1);
      if(test_oracle.test_message_for_cycle(msg1)) {
        std::cout << "Loop Detected while adding message" << std::endl;
        std::cout << "Removing first message" << std::endl;
        test_oracle.remove_message(my_vector[0]);
        if(!test_oracle.test_message_for_cycle(msg1)) {
          std::cout << "Sucuess loop removed" << std::endl;
        } else {
          std::cout << "Error loop still exists!" << std::endl;
        }
      } else {
        std::cout << "Sucuesfully added message" << std::endl;
        test_oracle.add_message(msg1);
      }     
  }
}

void loop_test() {
 std::vector<Message*> my_vector;
 Oracle test_oracle;
 uint32_t src_arr [2] =  {13, 45};
 uint32_t dest_arr [2] = {51, 21 };
 uint32_t addr_arr [2] = {0x00300000, 0x2FFD3000};
   for(int i = 0; i < 2; i++) {
		  uint32_t uid = get_uid(); 
      std::cout << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], MESSAGE_SIZE, 1, addr_arr[i], 3);
      my_vector.push_back(msg1);
      if(test_oracle.test_message_for_cycle(msg1)) {
        std::cout << "Loop Detected while adding message" << std::endl;
        std::cout << "Removing first message" << std::endl;
        test_oracle.remove_message(my_vector[0]);
        if(!test_oracle.test_message_for_cycle(msg1)) {
          std::cout << "Sucuess loop removed" << std::endl;
        } else {
          std::cout << "Error loop still exists!" << std::endl;
        }
      } else {
        std::cout << "Sucuesfully added message" << std::endl;
        test_oracle.add_message(msg1);
      }     
  }
}

void loop_test_harder() {
 std::vector<Message*> my_vector;
 Oracle test_oracle;
 uint32_t src_arr [2] =  {18, 18};
 uint32_t dest_arr [2] = {50, 35};
 uint32_t addr_arr [2] = {0x408C0000, 0x502AF530};
   for(int i = 0; i < 2; i++) {
		  uint32_t uid = get_uid(); 
      std::cout << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], MESSAGE_SIZE, 1, addr_arr[i], 3);
      my_vector.push_back(msg1);
      if(test_oracle.test_message_for_cycle(msg1)) {
        std::cout << "Loop Detected while adding message" << std::endl;
        std::cout << "Removing first message" << std::endl;
        test_oracle.remove_message(my_vector[0]);
        if(!test_oracle.test_message_for_cycle(msg1)) {
          std::cout << "Sucuess loop removed" << std::endl;
        } else {
          std::cout << "Error loop still exists!" << std::endl;
        }
      } else {
        std::cout << "Sucuesfully added message" << std::endl;
        test_oracle.add_message(msg1);
      }     
  }
}

void loop_test_harder1() {
 std::vector<Message*> my_vector;
 Oracle test_oracle;
 uint32_t src_arr [2] =  {18, 18};
 uint32_t dest_arr [2] = {50, 35};
 uint32_t addr_arr [2] = {0x408C0000, 0x502AF530};
   for(int i = 0; i < 2; i++) {
		  uint32_t uid = get_uid(); 
      std::cout << "Adding Message: uid: " << uid << " source: " << src_arr[i] <<" dest: " << dest_arr[i] << std::endl;      
      Message* msg1 = new Message(uid, src_arr[i], dest_arr[i], MESSAGE_SIZE, 1, addr_arr[i], 3);
      my_vector.push_back(msg1);
      if(test_oracle.test_message_for_cycle(msg1)) {
        std::cout << "Loop Detected while adding message" << std::endl;
        std::cout << "Removing first message" << std::endl;
        test_oracle.remove_message(my_vector[0]);
        if(!test_oracle.test_message_for_cycle(msg1)) {
          std::cout << "Sucuess loop removed" << std::endl;
        } else {
          std::cout << "Error loop still exists!" << std::endl;
        }
      } else {
        std::cout << "Sucuesfully added message" << std::endl;
        test_oracle.add_message(msg1);
      }     
  }
}

/*
int main(int argc, char** argv) {
  loop_test_harder(); 
  
  return 0;
}
*/
