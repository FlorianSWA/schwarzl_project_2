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
    // Mutex for vector update
    std::mutex update_mutex;

    std::mutex fail_check_mtx;

    // Update distance vector with new values
    void update_vector(int target_, int new_distance_);

    // Prints current vector entry to debug output
    void print_debug();

  public:
    DistanceVector(int port_, std::vector<int> neighbours_, int failed_conn_) {
      this->port = port_;
      this->neighbours = neighbours_;
      this->failed_connection = failed_conn_;
    };

    // vector of all neighbouring nodes
    std::vector<int> neighbours;

    // port of this node
    int port;

    // port of the neighbour whose connection failed
    int failed_connection;

    // flag to start simulating connection failure
    bool start_failure{false};

    // stores vector entries
    std::vector<VectorEntry> vector_storage;

    // number of entries in vector
    int vector_size = 0;

    // initializes vector with neighbour nodes
    void init();

    // Add entry to distance vector
    void add_or_update_entry(int target_port_, int next_hop_, int distance_);

    // Parse vector update and change adjust distance vector
    void parse_vector_update(int source_port_, proto_messages::VectorUpdate update_);

    // Returns the next hop for given target
    int get_next_hop(int target_);

    // sets disttance to maximum, indicating connection error
    void set_error_distance(int target_port_);

    // returns true if the targets distance is not set to infinite
    bool is_reachable(int target_port_);

    // returns true if port is simulating failure
    bool check_for_failure(int port_);

    // sets start_failure to true
    void start_fail();
};