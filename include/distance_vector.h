/*
Author: Florian Schwarzl
Date: 2021-03-21

Description: Class holding the distance vector with utilities to access it.
*/

#pragma once

#include "vector_entry.h"
#include <vector>

class DistanceVector {
  private:
    std::vector<VectorEntry> vector_storage;
  public:
    // Add entry to distance vector
    void add_entry(VectorEntry entry_);

    // Returns the next hop for given target
    int get_next_hop(int target_);

    // Update distance vector with new values
    void update_vector(int target_, int new_distance_);
};