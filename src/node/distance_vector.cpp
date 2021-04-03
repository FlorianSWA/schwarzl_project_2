/*
Author: Florian Schwarzl
Date: 2021-03-21

Description: Class holding the distance vector with utilities to access it.
*/

#include <sstream>
#include "distance_vector.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"


using namespace std;

void DistanceVector::add_or_update_entry(int target_port_, int next_hop_, int distance_) {
    bool found{false};
    if (target_port_ != this->port) {
        for (int i{0}; i < this->vector_size; i++) {
            if (vector_storage[i].target == target_port_) {
                found = true;
                if (distance_ < vector_storage[i].distance) {
                    VectorEntry entry;
                    entry.target = target_port_;
                    entry.next_hop = next_hop_;
                    entry.distance = distance_;
                    this->vector_storage[i] = entry;
                    spdlog::debug("Update distance vector entry to for {} to (target={}, next_hop={}, distance={})", target_port_, target_port_, next_hop_,  distance_);
                }
            }
        }
        
        if (!found) {
            VectorEntry entry;
            entry.target = target_port_;
            entry.next_hop = next_hop_;
            entry.distance = distance_;
            this->vector_storage.push_back(entry);
            this->vector_size += 1;
            spdlog::debug("Add new distance vector entry (target={}, next_hop={}, distance={})", target_port_, next_hop_,  distance_);
        }
    }
    stringstream debug_output_strm;
    for (int i{0}; i < vector_size; i++) {
        debug_output_strm << "\n (target=" << this->vector_storage[i].target << ", next_hop=" << this->vector_storage[i].next_hop << ", distance=" << this->vector_storage[i].distance << ")";
    }
    spdlog::debug(debug_output_strm.str());
}

void DistanceVector::parse_vector_update(int source_port_, proto_messages::VectorUpdate update_) {
    spdlog::info("Parsing vector update.", this->port);
    for (int i{0}; i < update_.vector_size(); i++) {
        proto_messages::VectorUpdate_VectorEntry v_entry{update_.vector(i)};
        add_or_update_entry(v_entry.target(), source_port_, v_entry.distance() + 1);
    }
}

void DistanceVector::init(vector<int> neighbours_) {
    for (size_t i{0}; i < neighbours_.size(); i++) {
        add_or_update_entry(neighbours_[i], neighbours_[i], 1);
    };
    add_or_update_entry(this->port, this->port, 0);
}

int DistanceVector::get_next_hop(int target_) {
    int next_hop{0};
    int shortest_distance{INT_MAX};
    for (int i{0}; i < vector_size; i++) {
        if (this->vector_storage[i].target == target_ && this->vector_storage[i].distance < shortest_distance) {
            next_hop = this->vector_storage[i].next_hop;
            shortest_distance = this->vector_storage[i].distance;
        }
    }
    spdlog::debug("Nexto");
    return next_hop;
}