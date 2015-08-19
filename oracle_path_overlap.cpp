#include "oracle_path_overlap.hpp"
#include <algorithm>
#include <iostream>


bool SortIntPairWithFirst (IntPair a, IntPair b) {return a.first < b.first;}
bool SortIntPairWithSecond (IntPair a, IntPair b) {return a.second < b.second;}


bool OraclePathOverlap::operator< (const OraclePathOverlap& right) const {
  if(_path1_uid == right._path1_uid) {
    return _path2_uid < right._path2_uid;
  } else {
    return _path1_uid < right._path1_uid;
  }
}


OraclePathOverlap::OraclePathOverlap(int uid1, int uid2, int hop_count1, int hop_count2) {
  _path1_uid = uid1;
  _path2_uid = uid2;
  _path1_hop_count = hop_count1;
  _path2_hop_count = hop_count2;
}


OraclePathOverlap::OraclePathOverlap(const OraclePathOverlap & clone_me) {
  _path1_uid = clone_me._path1_uid;
  _path2_uid = clone_me._path2_uid;
  _indexes_of_overlapping_paths = clone_me._indexes_of_overlapping_paths;
  _path1_hop_count = _path1_hop_count;
  _path2_hop_count = _path2_hop_count;
}

OraclePathOverlap::OraclePathOverlap() {

}

int OraclePathOverlap::get_other_uid(int uid) {
  if(uid == _path1_uid) {
    return _path2_uid;
  } else if(uid == _path2_uid) {
    return _path1_uid;
  } else {
    std::cout << "Error uid does not match either of the uids in the OraclePathOverlap" << std::endl;
    return -1;
  }
}

void OraclePathOverlap::add_overlapping_indexes(int index1, int index2) {
  _indexes_of_overlapping_paths.push_back(std::make_pair(index1, index2));
}  

int OraclePathOverlap::first_intesercting_index(int uid) {
  if(uid == _path1_uid) {
    return _indexes_of_overlapping_paths[0].first;  
  } else if(uid == _path2_uid) {
    return _indexes_of_overlapping_paths[0].second;
  } else {
    std::cout << "Error uid does not match either of the uids in the OraclePathOverlap" << std::endl;
    return -1;
  }
}

int get_from_pair(IntPair pair, int select) {
  if(select == 0)
    return pair.first;
  else
    return pair.second;
}

int OraclePathOverlap::get_lowest_index_downstream_path(int uid, int index) {
  std::cout << "lowest index with size of shared_tiles: " << _indexes_of_overlapping_paths.size() << std::endl;
  int select = 0;
  if(uid == _path1_uid) {
    std::sort (_indexes_of_overlapping_paths.begin(), _indexes_of_overlapping_paths.end(), SortIntPairWithFirst);  
    select = 0;
  } else if(uid == _path2_uid) {
    std::sort (_indexes_of_overlapping_paths.begin(), _indexes_of_overlapping_paths.end(), SortIntPairWithSecond);  
    select = 1;
  } else {
    std::cout << "Error no matching path in oracle_path_overlap" << std::endl;
    return -1;
  }
  // Start and end iterator tricks to ensure that overlapping paths are going in the same direction
  VectIntPairIter startIter = _indexes_of_overlapping_paths.end();
  VectIntPairIter endIter = _indexes_of_overlapping_paths.end();
  for(VectIntPairIter iter = _indexes_of_overlapping_paths.begin(); iter != _indexes_of_overlapping_paths.end(); ++iter) {
      std::cout << "start iter with indexes " << get_from_pair(*iter, select) << ", " << get_from_pair(*iter, !select) << std::endl;
    if(index <= get_from_pair(*iter, select) && (startIter == _indexes_of_overlapping_paths.end())) {
      startIter = iter;
    } else if(startIter != _indexes_of_overlapping_paths.end()) {
          endIter = iter;
          if(get_from_pair(*startIter, select) < get_from_pair(*endIter, select)) {
            if(get_from_pair(*startIter, !select) < get_from_pair(*endIter, !select)) {
              return get_from_pair(*startIter, !select);
            } 
          } else {
            std::cout << "Error should always be in order" << std::endl;
          }
    }   

  }
  return -1;
}

OraclePathOverlapIntersection OraclePathOverlap::get_indexes_of_lowest_downstream_intersection(int uid, int index) {
  std::cout << "lowest index with size of shared_tiles: " << _indexes_of_overlapping_paths.size() << std::endl;
  int select = 0;
  if(uid == _path1_uid) {
    std::sort (_indexes_of_overlapping_paths.begin(), _indexes_of_overlapping_paths.end(), SortIntPairWithFirst);  
    select = 0;
  } else if(uid == _path2_uid) {
    std::sort (_indexes_of_overlapping_paths.begin(), _indexes_of_overlapping_paths.end(), SortIntPairWithSecond);  
    select = 1;
  } else {
    std::cout << "Error no matching path in oracle_path_overlap" << std::endl;
    return DefaultOraclePathOverlapIntersection;
  }
  // Start and end iterator tricks to ensure that overlapping paths are going in the same direction
  VectIntPairIter startIter = _indexes_of_overlapping_paths.end();
  VectIntPairIter endIter = _indexes_of_overlapping_paths.end();
  for(VectIntPairIter iter = _indexes_of_overlapping_paths.begin(); iter != _indexes_of_overlapping_paths.end(); ++iter) {
      std::cout << "start iter with indexes " << get_from_pair(*iter, select) << ", " << get_from_pair(*iter, !select) << std::endl;
    if(index <= get_from_pair(*iter, select) && (startIter == _indexes_of_overlapping_paths.end())) {
      startIter = iter;
    } else if(startIter != _indexes_of_overlapping_paths.end()) {
          endIter = iter;
          if(get_from_pair(*startIter, select) < get_from_pair(*endIter, select)) {
            if(get_from_pair(*startIter, !select) < get_from_pair(*endIter, !select)) {
              OraclePathOverlapIntersection intersection;
              intersection.path_uid = uid;
              intersection.neighbor_uid = get_other_uid(uid);
              intersection.path_index = get_from_pair(*startIter, select);
              intersection.neighbor_index = get_from_pair(*startIter, !select);
              return intersection;
            } 
          } else {
            std::cout << "Error should always be in order" << std::endl;
          }
    }   

  }
  return DefaultOraclePathOverlapIntersection;
}
bool OraclePathOverlap::path_directions_aligned(IntPair start, IntPair end) {
  return ((start.first < end.first) && (start.second < end.second));
}


int OraclePathOverlap::get_path_type(int uid) {
 int select = 0;
 int path_hop_count = -1; 
 if(uid == _path1_uid) {
    std::sort (_indexes_of_overlapping_paths.begin(), _indexes_of_overlapping_paths.end(), SortIntPairWithFirst);  
    select = 0;
    path_hop_count = _path1_hop_count;
  } else if(uid == _path2_uid) {
    std::sort (_indexes_of_overlapping_paths.begin(), _indexes_of_overlapping_paths.end(), SortIntPairWithSecond);  
    select = 1;
    path_hop_count = _path2_hop_count;
  } else {
    std::cout << "Error no matching path in oracle_path_overlap" << std::endl;
    return -1;
  }
  
  VectIntPairIter temp_iter = _indexes_of_overlapping_paths.begin();
 
  VectIntPairIter lowest_start = _indexes_of_overlapping_paths.begin();
  VectIntPairIter lowest_end;
  if(_indexes_of_overlapping_paths.size() > 1) {
     lowest_end = ++temp_iter; 
  } else {
    std::cout << "Only one tile overlap is insufficient to create conflict" << std::endl;
  }
  VectIntPairIter highest_start = lowest_start;
  VectIntPairIter highest_end  = lowest_end; 
  temp_iter++; 

  while(temp_iter != _indexes_of_overlapping_paths.end()) {
    highest_start++;
    highest_end++;
    temp_iter++;
  }
  bool start_overlap = false;
  bool end_overlap = false;

  if(get_from_pair(*lowest_start, select) == 0) {
    if(!path_directions_aligned(*lowest_start, *lowest_end)) {
      std::cout << "paths dont align" << std::endl;
    } else {
      std::cout << "overlap at start" << std::endl;
      start_overlap = true;
    }
  }
  if(get_from_pair(*highest_end, select) == path_hop_count) { // size -1 = hop_count
    if(!path_directions_aligned(*highest_start, *highest_end)) {
      std::cout << "paths dont align" << std::endl;
    } else {
      std::cout << "overlap at end" << std::endl;
      end_overlap = true;
    }
  }

  if(start_overlap && (get_from_pair(*highest_end, select) == 1)) { // if the only overlap is at the top
    return UPSTREAM_CONFLICT;
  } else if(end_overlap &&(get_from_pair(*lowest_start, select) == path_hop_count - 1)) {
    return DOWNSTREAM_CONFLICT; 
  } else if((get_from_pair(*lowest_start, select) < get_from_pair(*highest_start, select)) && // if intersections from one pat are in increasing order but create
            (get_from_pair(*lowest_start, !select) > get_from_pair(*highest_start, !select))) {
    std::cout << "Cycle Detected" << std::endl;
    return CYCLE; 
  } else {
    return MERGE_AND_SPLIT;
  }
  return NO_CONFLICT;
}
 

