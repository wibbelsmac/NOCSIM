// my first program in C++
#include <iostream>
#include "routing_table.hpp"

int main()
{
  RoutingTable T(8,8,1,1);


  int src = 1;
  int dest = 16;

  int src1 = 1;
  int dest1 = 50;

  int src2 = 0;
  int dest2 = 27;
  
  int src3 = 0;
  int dest3 = 19;
  
  uint32_t hop_count;
  std::cout << "Begining Test:" << std::endl;
  std::cout << std::hex << T.generate_route_addr32(src, dest, &hop_count,1) << std::endl;
  std::cout << std::hex << T.generate_route_addr32(src1, dest1, &hop_count,1) << std::endl;
  std::cout << std::hex << T.generate_route_addr32(src2, dest2, &hop_count,1) << std::endl;
  std::cout << std::hex << T.generate_route_addr32(src3, dest3, &hop_count,1) << std::endl;
  
}

