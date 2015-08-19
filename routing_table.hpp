#ifndef ROUTINGTABLEH
#define ROUTINGTABLEH

#include <string>
#include <stdint.h>

// CHECK this when running testbench
#define NORTH "01"
#define SOUTH "10"
#define EAST "00"
#define WEST "11"

#define NORTH_bit 1
#define SOUTH_bit 2
#define EAST_bit 0
#define WEST_bit 3


// packet direction
#define XY 0 
#define ZIG_ZAG 1


class RoutingTable {
	int width, height, hop_cost, long_hop_cost;
	int x_loop_cost;
	int y_loop_cost;
	std::string print_route_addr(int x_delta, int y_delta);
	uint32_t generate_route_addr_bits(int x_delta, int y_delta);
	uint32_t generate_zig_zag_route_addr_bits(int x_delta, int y_delta);
  void address_test(uint32_t addr, int hops);
public:
  uint32_t generate_route_addr32(int source, int desti, uint32_t* hop_count, int mode);
	RoutingTable(int width, int height, int hop_cost, int long_hop_cost);
	std::string route(int x_start, int y_start, int x_end, int y_end);

};

#endif
