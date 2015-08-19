#include "sd_traffic_manager.hpp"

#include <stdio.h>

void TrafficManager::generate_messages() {
	msg_table.clear();	//initialize variables
	int width = 8;	//remove these
	int height = 8; //later
	int total_cores = width*height;
	int num_cores = rand_range(0, total_cores-1);
	for (int i=0; i<num_cores; i++)
		{
			message msg;
			msg.tx = rand_range(0, total_cores-1);
			msg.rx = rand_range(0, total_cores-1);
			if (msg.tx == msg.rx)
			{msg.rx = (msg.rx+1)%num_cores}	//something random
			else continue;
			
		}
	
}


unsigned int rand_range(unsigned int min, unsigned int max)
{
	double scaled = (double)rand()/RAND_MAX;
	return (max - min + 1)*scaled +min;
}
