#ifndef ORACLE_PATH_H
#define ORACLE_PATH_H

#include <map>
#include <set>
#include <vector>
#include <stdint.h>
#include "defines.hpp"
#include "oracle_path_overlap.hpp"

typedef std::set<OraclePathOverlap> OraclePathOverlapSet;
typedef OraclePathOverlapSet::iterator OraclePathOverlapSetIterator;
typedef std::set<OraclePathOverlapIntersection> SearchSet; // uid, index for entering OraclePath

struct sort_oracle_path_overlap { 
  bool operator() (OraclePathOverlap& a, OraclePathOverlap& b) const{
   if(a.uid1() == b.uid1()) {
     return a.uid2() < b.uid2();
   } else  {
     return a.uid1() < b.uid1();
   } 
  }

};

class OraclePath {
  std::vector<int> _covered_tiles;
  std::map<int, int> _covered_tile_to_path_index;
  std::set<OraclePathOverlap> _upstream_neighbors;
  std::set<OraclePathOverlap> _downstream_neighbors;
  std::map<int, OraclePathOverlap> _neighbors;
  int _uid, _src, _dest;
  uint32_t _address;
  void calculate_covered_tiles(uint32_t addr, int src, int dest); 
  public:
    OraclePath(int uid, uint32_t address, int source, int dest);
    ~OraclePath();
    std::vector<int> get_covered_tiles() { return _covered_tiles;};
    std::set<OraclePathOverlap> get_neighbors_downstream_from_index(int index);
    SearchSet get_indexes_of_neighbors_downstream_from_index(int index);
    SearchSet get_down_stream_search_set();
    bool get_indexes_of_neighbors_between_indexes(SearchSet& search_set, int start_index, int end_index);
    int index_of_overlapping_subpath(std::vector<int>& path, int starting_index);
    int get_index_of_first_overlap_with_neighbor(int neighbor_uid);
    int get_index_of_tile(int tile); 
    void add_oracle_path_overlap(OraclePathOverlap oracle_path_overlap);
    OraclePathOverlapSet get_upstream_neighbors() { return _upstream_neighbors; };
    OraclePathOverlapSet get_downstream_neighbors() { return _downstream_neighbors; };
    void remove_oracle_path(int uid); 
    int hop_count() {return _covered_tiles.size() -1; };
    int uid() {return _uid;};
    void clear_neighbors();
    int overlap_type_with_neighbor(int neighbor_uid);
    std::map<int, OraclePathOverlap> & get_neighbors() { return _neighbors; };
    OraclePathOverlap get_overlap_with_neighbor(int neighbor_uid);
};
#endif
