#include "stdio.h" 
//#include "vc_hdrs.h" 
#include "svdpi.h" 
#include "traffic_manager.hpp"
#include <stdint.h>
#include "defines.hpp"

std::ofstream ERROR_FILE;
std::ofstream LOG_FILE;
std::ofstream RESULTS_FILE;

std::string ERROR_FILENAME;
std::string LOG_FILENAME;
std::string RESULTS_FILENAME;
std::string SIMULATION_IDENTIFIER;
std::string TOPOLOGY;
std::string TRAFFIC_PATTERN;
int INJECTION_RATE = 10;
int MESSAGE_SIZE = 10;
int MESSAGES_IN_SIMULATION;
int MESSAGES_IN_WARMUP;
int MESSAGES_IN_MEASUREMENT;
int SHORT_HOP_COST;
int LONG_HOP_COST;
int ROUTE_MODE;


extern "C" { 

//std::ofstream ERROR_FILE;
//std::ofstream LOG_FILE;
//std::ofstream RESULTS_FILE;

typedef struct{ 
int a; 
int b; 

} ShitFlit; 

int string_sv2c(const char* str){ 

  printf(" C: %s",str); 
  return 0; 

}


extern void export_func(svBitVec32 x[3] ); 
//extern int64_t simulation_time();

void* get_tm() {
  return new TrafficManager(WIDTH,HEIGHT, MESSAGES_IN_SIMULATION);
}

int message_generation_period() {
  return INJECTION_RATE * MESSAGE_SIZE;
}

void init (const char* log_filename, const char*  error_filename, const char*  results_filename,
           const char* sim_id, const char* topology, const char* trfic_ptrn, int injection_rate,
    int message_size, int messages_in_sim, int messages_in_warm, int messages_in_meas, int short_hop_cost, int long_hop_cost, int route_mode) {
 // printf("Running Initialization method\n");
 // printf("%s, %s, %s, %s, %s, %s, %d, %d, %d, %d, %d, %d, %d", log_filename, error_filename, results_filename, sim_id, topology, trfic_ptrn, injection_rate, message_size,
 //        messages_in_sim, messages_in_warm, messages_in_meas, short_hop_cost, long_hop_cost);
  
  
  
  LOG_FILENAME = log_filename;
  ERROR_FILENAME = error_filename;
  RESULTS_FILENAME = results_filename;
  INJECTION_RATE = injection_rate;
  MESSAGE_SIZE = message_size;
  MESSAGES_IN_SIMULATION = messages_in_sim;
  MESSAGES_IN_WARMUP = messages_in_warm;
  MESSAGES_IN_MEASUREMENT = messages_in_meas;
  SHORT_HOP_COST = short_hop_cost;
  LONG_HOP_COST = long_hop_cost;
  ROUTE_MODE = route_mode;
  SIMULATION_IDENTIFIER = sim_id;
  TOPOLOGY = topology;
  TRAFFIC_PATTERN = trfic_ptrn;
  ERROR_FILE.open(ERROR_FILENAME.c_str(), std::ofstream::out | std::ofstream::trunc);
  LOG_FILE.open(LOG_FILENAME.c_str(), std::ofstream::out | std::ofstream::trunc);
  //RESULTS_FILE.open(RESULTS_FILENAME.c_str(), std::ofstream::out | std::ofstream::trunc);
  //printf("Ccompleting Initialization method\n");
  
}

void terminate(TrafficManager* TM) {
 TM->print_queue_stats();
 if(!TM->simulation_complete()) {
  TM->log_incomplete_messages();
 }
 TM->analyze_messages();

  ERROR_FILE.close();
  LOG_FILE.close();
  //RESULTS_FILE.close();
}

int simulation_complete (TrafficManager* TM) {
  /*if(TM->simulation_complete()) {
    //printf("ITS DONE\n");
  } else {
    printf("NOT DONE\n");
  }*/
  //printf("Returning %d", TM->simulation_complete());
  return TM->simulation_complete();
}

void generate_messages(TrafficManager* TM, long long int run_time) {
  //printf("Running Generate Messages method at time %lld\n", run_time);
	TM->generate_messages(run_time);
}

int has_packets_in_queue(TrafficManager* TM, int index, long long int run_time) {
  //printf("checking has method at time %lld\n", run_time);
  return TM->pending_socket_message(index, run_time);
}

void add_packet_to_network(TrafficManager* TM, int index, long long int run_time, NetworkFlit* flit) {
 //printf("checking has method at time %lld\n", run_time);
 NetworkFlit NF = TM->adding_message_to_network(index, run_time);
 flit->address = NF.data; // weird dpi flipping thing that we cant explain
 flit->data = NF.address;
}

void remove_packet_from_network(TrafficManager* TM, int64_t run_time, int uid, int index) {
 printf("checking has method at time %lld\n", run_time);
  TM->removing_message_from_network(uid, index,run_time);
}

} 



