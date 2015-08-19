#include "traffic_analyzer.hpp"
#include "message.hpp"
#include <stdio.h>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "defines.hpp"
#include <iostream>     // std::cout, std::fixed
#include <iomanip>


extern "C" {


TrafficAnalyzer::TrafficAnalyzer() {
 filename = "simulation_results.txt";  
}


void TrafficAnalyzer::analyze_message_queue(MessageQueue* message_queue, uint32_t injection_rate, uint32_t min_uid, uint32_t max_uid) {
  double message_count = 0;
  int analyzed_messages = 0;
  double measured_flit_count = 0.0;
  double total_hop_count = 0;
  double total_message_latency = 0;
  double min_message_latency_per_hop = 0;
  double max_message_latency_per_hop = 0;
  double cumulitave_offered_throughput = 0;
  double cumulitave_actual_throughput = 0;
  double avg_actual_throughput =0.0;
  double avg_offered_throughput = 0.0;
  double avg_message_latency_per_hop = 0.0;
  double actual_throughput = 0;
  double offered_throughput = 0; 
  double min_transmission_time_per_hop = 0;
  double max_transmission_time_per_hop = 0;
  double avg_transmission_time_per_hop = 0;
  double min_flit_latency_per_hop = 0.0;
  double max_flit_latency_per_hop = 0.0;
  double total_flit_latency = 0.0;
  double avg_flit_latency_per_hop = 0.0;
  double non_measured_flit_count = 0.0;
  MessageMap* analysis_map = message_queue->gimme_the_message_map();
  for(MessageMapIter iter = analysis_map->begin(); iter != analysis_map->end(); iter++) {
    
    Message* msg = iter->second;
    if(msg->all_flits_received()) { 
    if(msg->uid() >= min_uid && msg->uid() <= max_uid) {
     measured_flit_count = measured_flit_count + double(msg->message_size());
     double latency = msg->latency();
     double hop_count = msg->total_hop_count();
     double latency_per_hop = latency / hop_count;
     double temp_min_flit_latency_per_hop = msg->min_flit_latency_per_hop();
     double temp_max_flit_latency_per_hop = msg->max_flit_latency_per_hop();
     double message_flit_latency = msg->total_flit_latency();
     msg->print_lat_info();
     //printf("Analyzing message with latency %lld latency %lld hop_count: %lf, and lat_per_hop %lf\n",msg->latency(),latency, hop_count, latency_per_hop);
     if(message_count == 0) {
      min_message_latency_per_hop = latency_per_hop;
      max_message_latency_per_hop = latency_per_hop; 
     }else {
      if(latency_per_hop < min_message_latency_per_hop) {
        min_message_latency_per_hop = latency_per_hop;
      } else if(latency_per_hop > max_message_latency_per_hop) {
        max_message_latency_per_hop = latency_per_hop;
      }
     }
     
     if(message_count == 0) {
      min_flit_latency_per_hop = temp_min_flit_latency_per_hop;
      max_flit_latency_per_hop = temp_max_flit_latency_per_hop;
     }else {
      if(temp_min_flit_latency_per_hop < min_flit_latency_per_hop) {
        max_flit_latency_per_hop = temp_max_flit_latency_per_hop;
      } else if(temp_max_flit_latency_per_hop > max_flit_latency_per_hop) {
        min_flit_latency_per_hop = temp_min_flit_latency_per_hop;
      }
     } 
     
     analyzed_messages++;
     message_count = message_count + 1.0;
     total_hop_count += hop_count;
     total_message_latency += latency;
     total_flit_latency = total_flit_latency +  message_flit_latency;
     offered_throughput = msg->offered_bandwidth(); 
     actual_throughput = msg->actual_bandwidth(); 
     printf("Offered bandwidth: %f\n", offered_throughput);
     
     printf("Actual bandwidth: %f\n", actual_throughput);
     
     cumulitave_offered_throughput = cumulitave_offered_throughput + offered_throughput; 
     cumulitave_actual_throughput = cumulitave_actual_throughput + actual_throughput;  
      
    } else {
      non_measured_flit_count = non_measured_flit_count + double(msg->message_size()); 
    }
    } 
  }
  
  if(analyzed_messages != 0) {
  avg_actual_throughput = cumulitave_actual_throughput / message_count;
  avg_offered_throughput = cumulitave_offered_throughput / message_count;
  avg_message_latency_per_hop = total_message_latency / total_hop_count;
  avg_flit_latency_per_hop = total_flit_latency / (measured_flit_count * total_hop_count); 
  }

  std::ifstream infile(RESULTS_FILENAME.c_str());
  bool file_exists = infile.good();
  infile.close();
  if(!file_exists)
    init_results_file();
 
    printf("Writing to File!\n");
  RESULTS_FILE.open (RESULTS_FILENAME.c_str(), std::ios::app);
  RESULTS_FILE << SIMULATION_IDENTIFIER <<  std::fixed << std::setprecision(8) 
  << "\t" <<  std::fixed << std::setprecision(8) << injection_rate
  << "\t" <<  std::fixed << std::setprecision(8) << (measured_flit_count + non_measured_flit_count) << "/"
  <<          std::fixed << std::setprecision(8) << (MESSAGES_IN_SIMULATION * MESSAGE_SIZE)
  << "\t" <<  std::fixed << std::setprecision(8) << total_hop_count
  << "\t" <<  std::fixed << std::setprecision(8) << min_message_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << max_message_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_message_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << min_flit_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << max_flit_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_flit_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_actual_throughput 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_offered_throughput 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_actual_throughput / avg_offered_throughput 
  << "\t" <<  std::fixed << std::setprecision(8) << total_hop_count 
  << std::endl;
  RESULTS_FILE.close();


  std::cout << SIMULATION_IDENTIFIER <<  std::fixed << std::setprecision(8) 
  << "\t" <<  std::fixed << std::setprecision(8) << injection_rate
  << "\t" <<  std::fixed << std::setprecision(8) << total_hop_count 
  << "\t" <<  std::fixed << std::setprecision(8) << (measured_flit_count + non_measured_flit_count) << "/" 
  <<          std::fixed << std::setprecision(8) << (MESSAGES_IN_SIMULATION * MESSAGE_SIZE)
  << "\t" <<  std::fixed << std::setprecision(8) << min_message_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << max_message_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_message_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << min_flit_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << max_flit_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_flit_latency_per_hop 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_actual_throughput 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_offered_throughput 
  << "\t" <<  std::fixed << std::setprecision(8) << avg_actual_throughput / avg_offered_throughput 
  << "\t" <<  std::fixed << std::setprecision(8) << total_hop_count 
  << std::endl;


}

void TrafficAnalyzer::init_results_file() {
  RESULTS_FILE.open(RESULTS_FILENAME.c_str(), std::ios::app);
  RESULTS_FILE <<  "Simulation_Name\t" << "injection_rate\t" << "RCVD/TOTAL\t" << "total_hop_count\t" << "min_message_latency_per_hop\t" << "max_message_latency_per_hop\t" << "avg_message_latency_per_hop\t";
  RESULTS_FILE <<"min_flit_latency_per_hop\t" << "max_flit_latency_per_hop\t" << "avg_flit_latency_per_hop\t"  << "avg_actual_throughput\t" << "avg_offered_throughput\t" << "avg_actual_throughput/avg_offered_throughputi\t" << "total_hop_count" <<  std::endl;
  RESULTS_FILE.close();
 
}

///void TrafficAnalyzer::generate_power_numbers() {

//}



}

