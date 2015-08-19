#include "routing_table.hpp"
#include "defines.hpp"
#include <bitset>
#include <stdlib.h>     /* abs */
#include <iostream>
#include <stdio.h>



RoutingTable::RoutingTable(int width, int height, int hop_cost, int long_hop_cost) {
 	this->width = width;
 	this->height = height;
 	this->hop_cost = hop_cost;
 	this->long_hop_cost = long_hop_cost;
 	this->x_loop_cost = (width-1) * hop_cost + long_hop_cost;
	this->y_loop_cost = (height-1) * hop_cost + long_hop_cost;
}


uint32_t RoutingTable::generate_route_addr32(int source, int dest, uint32_t* hop_count, int mode) {
  int x_diff = (dest / height) - (source / height);
  int y_diff = (dest % height) - (source % height);
  int x_delta = x_diff;
	int y_delta = y_diff;
	if(abs(x_diff) >= (x_loop_cost / 2)) {
		x_delta = x_loop_cost - abs(x_diff);
		if(x_diff > 0) { // faster route is left instead of right
			x_delta *= -1;
		}

	}

	if(abs(y_diff) >= (y_loop_cost / 2)) {
		y_delta = y_loop_cost - abs(y_diff);
		if(y_diff > 0) { // faster route is down instead of up
			y_delta *= -1;
		}

	}
  *hop_count = abs(x_delta) + abs(y_delta);
  //std::cout << "x_delta " << x_delta << " Y_delta " << y_delta <<std::endl;
  printf("Creating addr bits wiht hop_count: %d, xdif: %d, ydif: %d\n", *hop_count, x_delta, y_delta);
	if(mode == XY)
    return generate_route_addr_bits(x_delta, y_delta);
  else if(mode == ZIG_ZAG) 
    return generate_zig_zag_route_addr_bits(x_delta, y_delta);
  else {
    //LOG_ERROR << "Unrecognized routing mode either choose XY routing or ZIG_ZAG in the mode" << std::endl;
    return 0;
  }
}



std::string RoutingTable::route(int x_start, int y_start, int x_end, int y_end) {
	int x_diff = x_end - x_start;
	int y_diff = y_end - y_start;
	int x_delta = x_diff;
	int y_delta = y_diff;
	if(abs(x_diff) >= (x_loop_cost / 2)) {
		x_delta = x_loop_cost - abs(x_diff);
		if(x_diff > 0) { // faster route is left instead of right
			x_delta *= -1;
		}

	}

	if(abs(y_diff) >= (y_loop_cost / 2)) {
		y_delta = y_loop_cost - abs(y_diff);
		if(y_diff > 0) { // faster route is down instead of up
			y_delta *= -1;
		}

	}
	return print_route_addr(x_delta, y_delta);
}

std::string RoutingTable::print_route_addr(int x_delta, int y_delta) {
//	std::string address ((abs(x_delta) + abs(y_delta)) * 2,  '0');
	std::string address (4 * 2,  '0');
	std::string x_direction_string = (x_delta > 0) ? EAST : WEST;
	std::string y_direction_string = (y_delta > 0) ? NORTH : SOUTH; 
	//std::cout << "x_delts" << x_delta << " Y_delta" << y_delta <<std::endl;
	int x;
	for(x = 0; x < abs(x_delta); x++) {
		address.replace(x*2, 2, x_direction_string);
	}
	for(uint y = abs(x_delta); y < (address.length()/2); y++) {
		address.replace(y*2, 2, y_direction_string);
	}
	return address;
}

uint32_t RoutingTable::generate_route_addr_bits(int x_delta, int y_delta) {
	uint32_t x_direction_bit = (x_delta > 0) ? EAST_bit : WEST_bit;
	uint32_t x_exit_direction_bit = (x_delta > 0) ? WEST_bit : EAST_bit;
	uint32_t y_direction_bit = (y_delta > 0) ? NORTH_bit : SOUTH_bit; 
	uint32_t y_exit_direction_bit = (y_delta > 0) ? SOUTH_bit : NORTH_bit;
  uint32_t exit_direction_bit = (y_delta == 0) ? x_exit_direction_bit : y_exit_direction_bit;
	uint32_t address = 0;
  //std::cout << "x_delts" << x_delta << " Y_delta" << y_delta <<std::endl;
	int x, y;
  int shift = 30;
	for(x = 0; x < abs(x_delta); x++) {
	  address |= (x_direction_bit << shift);
    shift = shift - 2;
    //printf("in x loop x_dir = %d", x_direction_bit);
  }
	for(y = 0; y < (abs(y_delta)); y++) {
		//address.replace(y*2, 2, y_direction_string);
	  address |= (y_direction_bit << shift);
    shift = shift - 2;

	}
	address |= (exit_direction_bit << shift);
	return address;
}

uint32_t RoutingTable::generate_zig_zag_route_addr_bits(int x_delta, int y_delta) {
	uint32_t x_direction_bit = (x_delta > 0) ? EAST_bit : WEST_bit;
	uint32_t x_exit_direction_bit = (x_delta > 0) ? WEST_bit : EAST_bit;
	uint32_t y_direction_bit = (y_delta > 0) ? NORTH_bit : SOUTH_bit; 
	uint32_t y_exit_direction_bit = (y_delta > 0) ? SOUTH_bit : NORTH_bit;
	uint32_t address = 0;
  std::cout << "x_delta " << x_delta << " Y_delta " << y_delta <<std::endl;
	int x = abs(x_delta);
  int y = abs(y_delta);
  uint32_t exit_direction_bit = (x > y) ? x_exit_direction_bit : y_exit_direction_bit;

  int first_delta_index, second_delta_index;
  uint32_t first_direction, second_direction;

  if(x >= y) {
    first_delta_index = x;
    first_direction = x_direction_bit;
    second_delta_index = y;
    second_direction = y_direction_bit;
  } else {
    first_delta_index = y;
    first_direction = y_direction_bit;
    second_delta_index = x;
    second_direction = x_direction_bit;
  }

  int shift = 30;
  while(first_delta_index > 0 || second_delta_index > 0) {
    if(first_delta_index > 0) {
      address |= (first_direction << shift);
      shift = shift - 2;
      //printf("in x loop x_dir = %d", x_direction_bit);
      first_delta_index--;
    }
    if(second_delta_index > 0) {
      //address.replace(y*2, 2, y_direction_string);
      address |= (second_direction << shift);
      shift = shift - 2;
      second_delta_index--;
    }
  }
	address |= (exit_direction_bit << shift);
  address_test(address, x + y);
  return address;
}

void RoutingTable::address_test(uint32_t addr, int hops) {
  uint32_t exit, last_dir;
  last_dir = (addr >> (32 - ((2 * hops) + 2))) & 3;
  exit = (addr >> (32 - (2 * hops))) & 3;
  if(exit != ((~last_dir) & 3))
    LOG_ERROR << "Incorrect exit direction in address: "  << " last direction: " << last_dir << " exit " << exit << std::endl;
}
