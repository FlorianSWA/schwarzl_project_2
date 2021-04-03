/*
Author: Florian Schwarzl
Date: 2021-03-21

Description: Class holding the distance vector with utilities to access it.
*/

#pragma once

#include <vector>
#include "proto_messages.pb.h"


struct VectorEntry{
    int target;
    int next_hop;
    int distance;
};

class DistanceVector {
  private:
    // Update distance vector with new values
    void update_vector(int target_, int new_distance_);

  public:
    DistanceVector(int port_) {
      this->port = port_;
    };

    // port of this node
    int port;

    // stores vector entries
    std::vector<VectorEntry> vector_storage;

    // number of entries in vector
    int vector_size = 0;

    // initializes vector with neighbour nodes
    void init(std::vector<int> neighbours_);

    // Add entry to distance vector
    void add_or_update_entry(int target_port_, int next_hop_, int distance_);

    // Parse vector update and change adjust distance vector
    void parse_vector_update(int source_port_, proto_messages::VectorUpdate update_);

    // Returns the next hop for given target
    int get_next_hop(int target_);
};