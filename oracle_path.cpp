#include "oracle_path.hpp"
#include "routing_table.hpp"
#include "oracle_path_overlap.hpp"

#include <utility>




OraclePath::OraclePath(int uid, uint32_t address, int source, int dest) {
  _src = source;
  _dest = dest;
  _uid = uid;
  calculate_covered_tiles(address, source, dest); 
}

OraclePath::~OraclePath() {
 
}

int OraclePath::get_index_of_tile(int tile) {
  if(_covered_tile_to_path_index.find(tile) != _covered_tile_to_path_index.end()) {
    return _covered_tile_to_path_index[tile]; 
  } else {
    std::cout << "Unable to find index for tile: " << tile << std::endl;
    return -1;
  }
} 


void OraclePath::calculate_covered_tiles(uint32_t addr, int src, int dest) {
 _covered_tiles.push_back(src);
 _covered_tile_to_path_index[src] = 0;
 int overlapped_tiles = 1;
 int count = WIDTH * HEIGHT; 
 uint32_t last_direction, cur_direction; 
 int shift = 30;
 cur_direction = ((addr >> shift) & 3);
 int cur_index = src;
 shift -=2;
 do {
  if(cur_direction == NORTH_bit) {
    int y = cur_index % HEIGHT;
    int x = cur_index / HEIGHT;
    y = (y+1) % HEIGHT;
    cur_index = x * HEIGHT + y;
  } else if(cur_direction == EAST_bit) {
    cur_index = (cur_index + HEIGHT) % count;
  } else if(cur_direction == SOUTH_bit) {
    int y = cur_index % HEIGHT;
    int x = cur_index / HEIGHT;
    if(y % HEIGHT == 0) {
      y += HEIGHT - 1;
    }else {
      y = y-1;
    }
    cur_index = x * HEIGHT + y;
  } else if(cur_direction == WEST_bit) {
    if(cur_index / HEIGHT == 0) {
      cur_index = cur_index + ((WIDTH - 1) * HEIGHT);
    } else {
      cur_index = cur_index - HEIGHT;
    }
  }
  last_direction = cur_direction;
  cur_direction = ((addr >> shift) & 3);
  shift -= 2;
  _covered_tiles.push_back(cur_index);
  _covered_tile_to_path_index[cur_index] = overlapped_tiles;
  overlapped_tiles++;
  if(shift < 2){
   //LOG_ERROR << "Covered TILES algorithms is not terminating" << std::endl;
   std::cout << "Covered TILES algorithms is not terminating" << std::endl;
  }
 } while(last_direction != ((~cur_direction) & 3));
  if(cur_index != dest) {
    std::cout << "Error:  incorrect calculation of dest" << std::endl;
  }
  std::cout << "Finished covered tiles with ";
  for(std::vector<int>::iterator it = _covered_tiles.begin(); it != _covered_tiles.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "tiles" << std::endl;
  std::cout << "Path to index: ";
  for(std::map<int,int>::iterator it = _covered_tile_to_path_index.begin(); it != _covered_tile_to_path_index.end(); ++it) {
    std::cout << " <" << it->first << ", " << it->second << "> ," ;
  }
  std::cout << std::endl;
}

/*
int OraclePath::index_of_overlapping_subpath(std::vector<int>& path, int starting_index) {
  int end = path.size();
  int overlapping_index = _covered_tile_to_path_index[path[starting_index]];
  for(int i = starting_index+1; i < end; i++) {
    if(_covered_tile_to_path_index.find(path[i]) != _covered_tile_to_path_index.end() &&
       _covered_tile_to_path_index[i] > starting_index) {
      return overlapping_index; 
    }
  }
  return -1;
}
*/

SearchSet OraclePath::get_indexes_of_neighbors_downstream_from_index(int index) {
  std::cout << "Neighbors upstream: " << _upstream_neighbors.size() << " downstream: " << _downstream_neighbors.size() << std::endl;
  std::cout << "Finding neighbors downstream of index " << index << std::endl;
  SearchSet search_set;
  for(OraclePathOverlapSetIterator iter = _downstream_neighbors.begin(); iter != _downstream_neighbors.end(); ++iter) {
    OraclePathOverlap temp_overlap = *iter;
    OraclePathOverlapIntersection intersection_temp;
    intersection_temp = temp_overlap.get_indexes_of_lowest_downstream_intersection(_uid,index);
    std::cout << "intersecting index with: " << intersection_temp.neighbor_uid << " at: " << intersection_temp.neighbor_index << std::endl;
    if(intersection_temp.path_intersection != -1) {
      search_set.insert(intersection_temp);
    }
  }
  return search_set;
}

bool OraclePath::get_indexes_of_neighbors_between_indexes(SearchSet& search_set, int start_index, int end_index) {
  std::cout << "Neighbors upstream: " << _upstream_neighbors.size() << " downstream: " << _downstream_neighbors.size() << std::endl;
  std::cout << "Finding neighbors downstream of index " << index << std::endl;
  for(OraclePathOverlapSetIterator iter = _downstream_neighbors.begin(); iter != _downstream_neighbors.end(); ++iter) {
    OraclePathOverlap temp_overlap = *iter;
    OraclePathOverlapIntersection intersection_temp;
    intersection_temp = temp_overlap.get_indexes_of_lowest_downstream_intersection(_uid,index);
    std::cout << "intersecting index with: " << intersection_temp.neighbor_uid << " at: " << intersection_temp.neighbor_index << std::endl;
    if(intersection_temp.path_index != -1 && intersection_temp.path_index < end_index) {
      search_set.insert(intersection_temp);
    }
  }
  return (search_set.size() > 0);
}


int OraclePath::overlap_type_with_neighbor(int neighbor_uid) {
  if(_neighbors.find(neighbor_uid) != _neighbors.end()) {
    return _neighbors[neighbor_uid].get_path_type(_uid);
  } else {
    std::cout << "Error looking for neighbor that doesnt exist check algorithm" << std::endl;
    return NO_CONFLICT; 
  }
}

void OraclePath::add_oracle_path_overlap(OraclePathOverlap oracle_path_overlap) {
  int path_type = oracle_path_overlap.get_path_type(_uid);
  int neighbor_uid = oracle_path_overlap.get_other_uid(_uid);
  _neighbors[neighbor_uid] = oracle_path_overlap; 
  if(path_type == NO_CONFLICT) {
    std::cout << "Error path does not overlap" << std::endl;
  } else if (path_type == UPSTREAM_CONFLICT) {
    _upstream_neighbors.insert(oracle_path_overlap);
  } else if (path_type == DOWNSTREAM_CONFLICT) {
    _downstream_neighbors.insert(oracle_path_overlap);
  } else if (path_type == MERGE_AND_SPLIT){
    _upstream_neighbors.insert(oracle_path_overlap);
    _downstream_neighbors.insert(oracle_path_overlap);
  } else if(path_type == CYCLE) {
    std::cout << "Error cycle created" << std::endl;
  }
    
}


void OraclePath::remove_oracle_path(int uid) {
for(OraclePathOverlapSetIterator it = _upstream_neighbors.begin(); it != _upstream_neighbors.end(); ++it) {
  OraclePathOverlap oracle_path_overlap_temp = *it;
  if(oracle_path_overlap_temp.get_other_uid(_uid) == uid) {
    _upstream_neighbors.erase(it);
  }
}
for(OraclePathOverlapSetIterator it = _downstream_neighbors.begin(); it != _downstream_neighbors.end(); ++it) {
  OraclePathOverlap oracle_path_overlap_temp = *it;
  if(oracle_path_overlap_temp.get_other_uid(_uid) == uid) {
    _downstream_neighbors.erase(it);
  }
}
}

OraclePathOverlap OraclePath::get_overlap_with_neighbor(int neighbor_uid) {
  return _neighbors[neighbor_uid]; 
}

void OraclePath::clear_neighbors() {
  _upstream_neighbors.clear();
  _downstream_neighbors.clear();
}

int OraclePath::get_index_of_first_overlap_with_neighbor(int neighbor_uid) {
  if(_neighbors.find(neighbor_uid) != _neighbors.end()) {
    std::cout << "Error could not find requested neighbor" << std::endl;
    return -1;
  } else {
    return _neighbors[neighbor_uid].first_intesercting_index(neighbor_uid);
  }
}

