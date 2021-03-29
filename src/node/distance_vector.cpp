/*
Author: Florian Schwarzl
Date: 2021-03-21

Description: Class holding the distance vector with utilities to access it.
*/

#include "distance_vector.h"
#include "vector_entry.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"


using namespace std;

void DistanceVector::add_or_update_entry(int target_port_, int next_hop_, int distance_) {
    bool found{false};
    for (int i{0}; i < vector_size; i++) {
        if (target_port_ != self) {
            if (vector_storage[i].target == target_port_) {
                found = true;
                if (vector_storage[i].distance < distance_) {
                    VectorEntry entry;
                    entry.target = target_port_;
                    entry.next_hop = next_hop_;
                    entry.distance = distance_;
                    vector_storage[i] = entry;
                    spdlog::debug("Update distance vector entry to for {} to (target={}, next_hop={}, distance={})", target_port_, target_port_, next_hop_,  distance_);
                }
            }
        }
    }
    
    if (!found) {
        VectorEntry entry;
        entry.target = target_port_;
        entry.next_hop = next_hop_;
        entry.distance = distance_;
        vector_storage.push_back(entry);
        vector_size += 1;
        spdlog::debug("Add new distance vector entry (target={}, next_hop={}, distance={})", target_port_, next_hop_,  distance_);
    }
}

void DistanceVector::parse_vector_update(int source_port_, proto_messages::VectorUpdate update_) {
    for (int i{0}; i < update_.vector_size(); i++) {
        proto_messages::VectorUpdate_VectorEntry v_entry{update_.vector(i)};
        add_or_update_entry(v_entry.target(), source_port_, v_entry.distance() + 1);
    }
}

void DistanceVector::init(vector<int> neighbours_) {
    for (size_t i{0}; i < neighbours_.size(); i++) {
        add_or_update_entry(neighbours_[i], neighbours_[i], 1);
    };
    add_or_update_entry(self, self, 0);
}

int DistanceVector::get_next_hop(int target_) {
    int next_hop{0};
    for (int i{0}; i < vector_size; i++) {
        if (vector_storage[i].target == target_) {
            next_hop = vector_storage[i].next_hop;
        }
    }
    return next_hop;
}