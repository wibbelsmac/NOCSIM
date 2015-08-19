#include "oracle1.hpp"
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
  // TODO add connected component search
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
  // TODO Add connected component search
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

void Oracle::update_message_neighbors(Message* message) {
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
}

void Oracle::add_upstream_neighbors_to_search_stack(OracleSearchStack & search_stack, Message* message) {
  std::set<OraclePathOverlap> upstream_neighbors_temp;
  upstream_neighbors_temp = new_path->get_upstream_neighbors();
   int upstream_index = -1;
   for(OraclePathOverlapSetIterator it = upstream_neighbors_temp.begin(); it != upstream_neighbors_temp.end(); ++it) {
     int neighbor_uid;
     //int downsream_index
     OraclePathOverlap op = (*it);
     neighbor_uid = op.get_other_uid(new_path->uid());
     downsream_index = op.first_intesercting_index(neighbor);
     //upstream_neighbors[neighbor] = *it;
     OracleSearchStruct search_srtuct;
     search_srtuct.uid = neighbor_uid;
     //search_srtuct.downstream_index = downsream_index;
     search_struct.upstream_index = upstream_index;
     search_stack.push(search_srtuct);  
   }

}


bool Oracle::additional_children_to_explore(SearchSet& search_set ParentDiscoveryMap& map, int uid, int index) {
  int previous_min_intersection = -1;
  if(map.find(uid) != map.end()) {
    for(PathDfsVect::iterator it = map[uid].begin(); it != map[uid].end(); ++it) {
      int potential_min = (*it).start_intersection;
      if(previous_min_intersection == - 1) {
        previous_min_intersection = potential_min;
      } else if(potential_min < previous_min_intersection) {
        previous_min_intersection = potential_min;
      }
    }
    if(index < previous_min_intersection) {
      return _path_map[uid]->downstream_paths_between_indexes(search_set, index, previous_min_intersection);
    } else {
      return false;
    } 
  } else {
    return true;
  }
}

bool Oracle::get_parent_dfs_struct(OraclePathDfsStruct* parent_dfs_struct, OraclePathDfsStruct* current_struct) {
 if(map.find(current_struct->uid) != map.end()) {
    PathDfsVect parent_intersection_vect = map[current_struct.parent_uid];
    PathDfsVect::iterator parent_iter = parent_intersection_vect.end(); 
    for(PathDfsVect::iterator it = parent_intersection_vect.begin(); it != parent_intersection_vect.end(); ++it) {
      if(parent_iter == map.end() && ((*it).start_intersection < current_struct->parent_index)) {
        parent_iter = it;  
      } else  if((*it.start_intersection >  parent_iter.start_intersection) && ((*it).start_intersection < current_struct->parent_index)) {
        parent_iter = it;
      }
    } 
    if(parent_iter != map.end()) {
      parent_dfs_struct = *parent_iter;
      return true;
    } else {
      return false;
    }
 } else {
  return false;
 }  
}


bool Oracle::find_oracle_path(OraclePath* oracle_path, int uid) {
  if(_path_map.find(uid_temp) != _path_map.end()) {
       oracle_path = _path_map[uid];
       return true;
   } else {
      std::cout << "Error searching for path that does not exist in map" << std::endl;
      return false; 
   } 
}

SearchSet Oracle::get_unexplored_indexes_of_neighbors_downstream_from_index(OracleSearchStruct& search_struct, OraclePath& oracle_path) {
  int search_index;
  if(search_struct.upstream_index == -1) {
      search_index = 0;
  } else {
      search_index = upstream_index;
  } 
  if(parent_discover_map.find(search_struct.uid) != parent_discover_map.end()) {
    PathDfsVect previously_explored_intersetcions = map[search_struct.uid];
    PathDfsVect::iterator intersection_iter = previously_explored_intersetcions.end(); 
    for(PathDfsVect::iterator it = previously_explored_intersetcions.begin(); it != previously_explored_intersetcions.end(); ++it) {
      if(intersection_iter == map.end() && ((*it).start_intersection > current_struct->parent_index)) {
        intersection_iter = it;  
      } else  if((*it.start_intersection <  intersection_iter.start_intersection) && ((*it).start_intersection > current_struct->parent_index)) {
        intersection_iter = it;
      }
    } 
    return oracle_path.get_indexes_of_neighbors_between_indexes(search_index, (*it).start_intersection);
      
  } else {
      return op_temp->get_indexes_of_neighbors_downstream_from_index(search_index);
  }
}


NodeType Oracle::identify_node_handle_downstream_neighbors(ParentDiscoveryMap& parent_discovery_map, OracleSearchStack & search_stack, OracleSearchStruct& search_struct, Message* message) {
   int parent uid = search_struct.uid;
   int search_index;
   bool has_unsearched_children = false;

   if(search_struct.upstream_index == -1) {
      search_index = 0;
   } else {
      search_index = upstream_index;
   }

   SearchSet ss_temp = oracle_path->get_indexes_of_neighbors_downstream_from_index(search_index);
   std::cout << "Found Search Set with size: " << ss_temp.size() << std::endl;
   for(SearchSet::iterator it = ss_temp.begin(); it != ss_temp.end(); ++it) {
     OraclePathIntersection path_intersection = *it;
     std::cout << "found neighbor with uid " << path_intersection.neighbor_uid << " conflicting at index: " << path_intersection.index << std::endl;
      

      // is not part of forest   
      if(parent_discovery_map.find(path_intersection.neighbor_uid) != parent_discovery_map.end()) {
        OracleSearchStruct child_struct = parent_discovery_map[path_intersection.neighbor_uid];
        int intersect; 
        if(child_struct.upstream_index == -1) {
          intersect = child_struct.downsream_index; 
        } else {
          intersect = child_struct.upstream_index;
        }
        std::cout << "Found previously visited path with uid: " << pre_queue_uid << " intersected at index: " << pre_queue_index << " intersecting at: " << intersect << std::endl;
        
        // determine if edge is front or back edge 
        // cases
        // 1. forward edge
        // 2. backward edge and no empty tokens
        //    return type
        //      parents completed indexes must be populated
        //      return false do not add message
        //
        // 3. backward edge and empty tokens
        //    return type
        //      parents completed indexes must be populated
        //      return true allow flit to be added but do not add message
        if(finished != -1) { // has edge that is not part of search forest but if index is less must check for additional search edges
            
        } else { // has backwards edge
          if(pre_queue_index < intersect) { // edge creates cycle
            std::cout << "cycle detected upstream of visited path" << std::endl;
            return NodeType.CYCLE;  
          }
        }
      } else { // Has children
        has_unsearched_children |= true;
        search_queue.push(*it);
      }
   }
  return false; 
}
 
}

void Oracle::visit_neighbor() {


}

void Oracle::populate_dependency_tree(ParentDiscoveryMap & parent_discover_map, Message* message) {
  int visited_index = 0;
  int search_index = -1; 
  
  update_message_neighbors(message);  
  OracleSearchStack stack;
  add_upstream_neighbors_to_search_stack(stack, message);  
   
  while(!stack.empty()) {
    OracleSearchStruct search_struct = stack.top();

    stack.pop();
    parent_discover_map[search_struct.uid] = DefaultOraclePathDfsStruct;
    parent_discover_map[search_struct.uid].upstream_index = search_struct.upstream_index;
    parent_discover_map[search_struct.uid].discovered = visited_index++;
  }
}

void Oracle::create_cycle_from_tree ( ) {
  
}
  
void Oracle::update_cycles ( ) {

}



bool Oracle::test_message_for_cycle(Message* message) {
  // dfs
  // detect if path is part of cycle
  
  // count number of empty tokens in cycle
  
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
