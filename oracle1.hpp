#ifndef ORACLE_H
#define ORACLE_H

#include <map>
#include <set>
#include <vector>
#include "defines.hpp"
#include "message.hpp"
#include "oracle_path.hpp"

enum NodeType {CYCLE, PARENT, CHILD};

struct path_intersection {
  int indexed_uid;
  int intersection_of_indexed_neighbor;
};

struct OracleSearchStruct {
  int uid;
  int parent_uid;
  int upstream_index;
  int downstream_index;
};

typedef OraclePathDfsStruct {
  int discovered;
  int finished;
  int parent_uid;
  int parent_index;
  int start_intersection;
  int end_intersection;
} DefaultOraclePathDfsStruct= {-1,-1,-1,-1,-1,-1};



typedef std::vector<OraclePathDfsStruct>> PathDfsVect;
typedef std::map<int,PathDfsVect> ParentDiscoveryMap;
typedef Stack<OracleSearchStruct> OracleSearchStack;

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
    void update_message_neighbors(Message* message);
    void add_upstream_neighbors_to_search_stack(OracleSearchStack & search_stack, Message* message);
    void populate_dependency_tree(Message* message);
    void create_cycle_from_tree();
    void update_cycles( );


}



};
#endif
