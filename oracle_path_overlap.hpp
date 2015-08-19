#ifndef ORACLE_PATH_OVERLAP_H
#define ORACLE_PATH_OVERLAP_H

#include <vector>
#include <utility>

#define NO_CONFLICT -1
#define UPSTREAM_CONFLICT 1
#define DOWNSTREAM_CONFLICT 2
#define MERGE_AND_SPLIT  3
#define CYCLE 4

typedef OraclePathOverlapIntersection {
  int path_uid;
  int path_index;
  int neighbor_uid;
  int neighbor_index;
} DefaultOraclePathOverlapIntersection = {-1,-1,-1,-1};

typedef std::pair<int,int> IntPair;
typedef std::vector< std::pair<int,int> >::iterator VectIntPairIter;



class OraclePathOverlap {
    int _path1_uid, _path2_uid;
    std::vector< std::pair<int,int> > _indexes_of_overlapping_paths;
    int _path1_hop_count, _path2_hop_count; 
    bool path_directions_aligned(IntPair start, IntPair end);
  public:
  OraclePathOverlap(int uid1, int uid2, int path1_hop_count, int path2_hop_count);
  OraclePathOverlap(const OraclePathOverlap& clone_me);
  OraclePathOverlap();
  int uid1() {return _path1_uid;};
  int uid2() {return _path2_uid;};
  void add_overlapping_indexes(int index1, int index2);
  OraclePathOverlapIntersection get_indexes_of_lowest_downstream_intersection(int uid, int index);
  int get_lowest_index_downstream_path(int uid, int index);
  int get_path_type(int uid); 
  int get_other_uid(int uid);
  int get_number_of_overlapping_tiles() { return _indexes_of_overlapping_paths.size(); };
  bool operator< (const OraclePathOverlap& right) const;
  int first_intesercting_index(int uid); 
  };
#endif
