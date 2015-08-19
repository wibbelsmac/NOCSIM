#include "oracle.hpp"
#include <iterator>
#include <queue>
#include <utility>


Oracle::Oracle() {
  _grid = new std::vector< std::set<OraclePath *> >(WIDTH * HEIGHT);
}
Oracle::~Oracle() {
  delete _grid;
}

void Oracle::add_neighbors_to_path(OraclePath* path) {
  find_overlapping_paths(path);
}
/*
void Oracle::remove_neighbors_from_path(OraclePath* path) {

}
*/
void Oracle::add_path_to_neighbors(OraclePath* path) {
  std::map<int, OraclePathOverlap> my_map = path->get_neighbors();
  for(std::map<int, OraclePathOverlap>::iterator it = my_map.begin(); it != my_map.end(); ++it) {
    OraclePathOverlap op = it->second;
    int uid = it->first;
    if(_path_map.find(uid) != _path_map.end())
    _path_map[uid]->add_oracle_path_overlap(op);
    else
      std::cout << "Error cannot find path in pathmap" << std::endl;
  }
}

void Oracle::remove_path_from_neighbors(OraclePath* path) {
  int path_uid = path->uid();
  std::map<int, OraclePathOverlap> my_map = path->get_neighbors();
  for(std::map<int, OraclePathOverlap>::iterator it = my_map.begin(); it != my_map.end(); ++it) {
    OraclePathOverlap op = it->second;
    int neighbor_uid = it->first;
    if(_path_map.find(neighbor_uid) != _path_map.end())
    _path_map[neighbor_uid]->remove_oracle_path(path_uid);
    else
      std::cout << "Error cannot find path in pathmap" << std::endl;
  } 
}

void Oracle::add_message(Message* message) {
  // calculate covered tiles build path
  // detect conflicts with other neighbors 
  // if no loop add to neighbors and add to grid
  //    add to overlapping paths and second_neighbors
  int uid = message->uid();
  OraclePath* new_path;
  if(_path_map.find(uid) == _path_map.end()) {
    //LOG_ERROR << "Attempting to add message that has not been previously tested for cycles" << std::endl;
    std::cout << "Attempting to add message that has not been previously tested for cycles" << std::endl;
  } else {
    new_path = _path_map[uid];
    add_path_to_grid_and_neighbors(new_path);
  }
}

void Oracle::remove_message(Message* message) {
  int uid = message->uid();
  if(_path_map.find(uid) != _path_map.end()) {
    remove_path_from_grid_and_neighbors(_path_map[uid]);
    OraclePath* op = _path_map[uid];
    _path_map.erase(uid);
    delete op;
  }
}

void Oracle::add_path_to_grid_and_neighbors(OraclePath* op) {
  std::vector<int> tiles = op->get_covered_tiles();  
  for(std::vector<int>::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    (*_grid)[*it].insert(op); 
  }
  add_path_to_neighbors(op);
}

void Oracle::remove_path_from_grid_and_neighbors(OraclePath* op) {
  std::vector<int> tiles = op->get_covered_tiles();  
  for(std::vector<int>::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    (*_grid)[*it].erase(op); 
  }
  remove_path_from_neighbors(op);

}


bool Oracle::test_message_for_cycle(Message* message) {
  int uid = message->uid();
  OraclePath* new_path;
  if(_path_map.find(uid) == _path_map.end()) {
    new_path = new OraclePath(uid, message->address(), message->source_index(), message->dest_index());
    add_neighbors_to_path(new_path);
    _path_map.insert(std::make_pair(message->uid(), new_path));
  } else {
    new_path = _path_map[uid];
    new_path->clear_neighbors();
    add_neighbors_to_path(new_path);
  }
  //std::map<int, OraclePathOverlap> upstream_neighbors;
  std::set<OraclePathOverlap> upstream_neighbors_temp;
  std::set<OraclePathOverlap> downstream_neighbors_temp;
  std::map<int, path_intersection> visited_path_uids;  // map of uid to intersections
  upstream_neighbors_temp = new_path->get_upstream_neighbors();
  for(OraclePathOverlapSetIterator it = upstream_neighbors_temp.begin(); it != upstream_neighbors_temp.end(); ++it) {
   int neighbor;
   int intersecting_index;
   OraclePathOverlap op = (*it);
   neighbor = op.get_other_uid(new_path->uid());
   intersecting_index = op.first_intesercting_index(neighbor);
   //upstream_neighbors[neighbor] = *it;
   path_intersection pi;
   pi.indexed_uid = neighbor;
   pi.intersection_of_indexed_neighbor = intersecting_index;
   visited_path_uids[neighbor] = pi;
  }
  std::queue<std::pair<int, int> > search_queue;
  search_queue.push(std::make_pair(uid, 0));
  // performing BFS 
  while(!search_queue.empty()) {
   IntPair uid_index = search_queue.front();
   search_queue.pop();
   OraclePath *op_temp;
   int uid_temp = uid_index.first;
   int index_temp = uid_index.second;
   path_intersection pi_temp;
   std::cout << "In search Queue with uid: " << uid_temp << " index: " << index_temp << std::endl; 
   pi_temp.indexed_uid = uid_temp;
   pi_temp.intersection_of_indexed_neighbor = index_temp;
   visited_path_uids.insert(std::make_pair(uid_temp, pi_temp));
   if(_path_map.find(uid_temp) != _path_map.end()) {
       op_temp = _path_map[uid_index.first];
   } else {
      std::cout << "Error searching for path that does not exist in map" << std::endl;
      return true; 
   }
   SearchSet ss_temp = op_temp->get_indexes_of_neighbors_downstream_from_index(index_temp);
   std::cout << "Found Search Set with size: " << ss_temp.size() << std::endl;
   for(SearchSet::iterator it = ss_temp.begin(); it != ss_temp.end(); ++it) {
     IntPair pre_queue_pair = *it;
     int pre_queue_uid = pre_queue_pair.first;
     int pre_queue_index = pre_queue_pair.second;
     std::cout << "found neighbor with uid " << pre_queue_uid << " conflicting at index: " << pre_queue_index << std::endl;
         
      if(visited_path_uids.find(pre_queue_uid) != visited_path_uids.end()) {
        int intersect = visited_path_uids[pre_queue_uid].intersection_of_indexed_neighbor;
        std::cout << "Found previously visited path with uid: " << pre_queue_uid << " intersected at index: " << pre_queue_index << " intersecting at: " << intersect << std::endl;
        if(pre_queue_index < intersect) {
          std::cout << "cycle detected upstream of visited path" << std::endl;
          return true;
        } 
        search_queue.push(*it);
        /*
        if(path_type == CYCLE) {
          return true;
          std::cout << "Cycle Overlapp Type Detected between only two paths" << std::endl;
        } else if(path_type == MERGE_AND_SPLIT) {
        
        } else {
          std::cout << "cycle_detected" << std::endl;
          return true;
        }
      */
        } else {
          search_queue.push(*it);
        }
      }
      
   }
  return false; 
}



void Oracle::find_overlapping_paths(OraclePath* path) {
  std::map<int, OraclePathOverlap> overlapping_paths;
  std::vector<int> covered_tiles = path->get_covered_tiles();
  for(std::vector<int>::iterator it = covered_tiles.begin(); it != covered_tiles.end(); ++it) {
    std::set<OraclePath*> * conflicting_paths = &((*_grid)[*it]);
    std::cout << "ovlaping path for tile: " << *it << std::endl;
    for(std::set<OraclePath *>::iterator path_iter = conflicting_paths->begin(); path_iter != conflicting_paths->end(); ++path_iter) {
      OraclePath* op_temp = *path_iter; 
      int conflicting_path_uid = op_temp->uid(); 
      if(overlapping_paths.find(conflicting_path_uid) != overlapping_paths.end()) {
        int overlapping_index = path->get_index_of_tile(*it);
        int neighbor_index = op_temp->get_index_of_tile(*it);
        overlapping_paths[conflicting_path_uid].add_overlapping_indexes(overlapping_index, neighbor_index);
      } else {
        int path_hop_count = path->hop_count();
        int neighbor_hop_count = op_temp->hop_count();
        std::cout << "detecting new overlapping path with uid1: " << path->uid() << " neighbor uid " << conflicting_path_uid << std::endl;
        OraclePathOverlap optemp (path->uid(), conflicting_path_uid, path_hop_count, neighbor_hop_count);
        int overlapping_index = path->get_index_of_tile(*it);
        int neighbor_index = op_temp->get_index_of_tile(*it);
        optemp.add_overlapping_indexes(overlapping_index, neighbor_index);
        overlapping_paths.insert(std::make_pair(conflicting_path_uid, optemp)); 
      }
    } 
  }
  for(std::map<int, OraclePathOverlap>::iterator conf_iter = overlapping_paths.begin(); conf_iter != overlapping_paths.end(); ++conf_iter) {
    OraclePathOverlap op_temp = conf_iter->second;
    if(op_temp.get_number_of_overlapping_tiles() >= 2) {
      path->add_oracle_path_overlap(conf_iter->second);
    }
  }
}
