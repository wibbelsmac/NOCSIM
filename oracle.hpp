#ifndef ORACLE_H
#define ORACLE_H

#include <map>
#include <set>
#include <vector>
#include "defines.hpp"
#include "message.hpp"
#include "oracle_path.hpp"

struct path_intersection {
  int indexed_uid;
  int intersection_of_indexed_neighbor;
};

class Oracle {
    std::vector< std::set<OraclePath *> > *_grid;    
    std::map<int, OraclePath *> _path_map;
    void add_neighbors_to_path(OraclePath* path);
    void remove_path_from_neighbors(OraclePath* path);
    void remove_path_from_grid_and_neighbors(OraclePath* path);
    void remove_neighbors_from_path(OraclePath* path); 
    void add_path_to_neighbors(OraclePath* path);
    void find_overlapping_paths(OraclePath* path);
    bool connected_paths(OraclePath* op1, OraclePath* op2);
    void add_path_to_grid_and_neighbors(OraclePath* op);
  public:
    Oracle();
    ~Oracle();
    void add_message(Message* message);
    void remove_message(Message* message);
    bool test_message_for_cycle(Message* message);

};
#endif
