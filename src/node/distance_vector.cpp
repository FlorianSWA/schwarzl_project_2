/*
Author: Florian Schwarzl
Date: 2021-03-21

Description: Class holding the distance vector with utilities to access it.
*/

#include <sstream>
#include "distance_vector.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"


using namespace std;

void DistanceVector::add_or_update_entry(int target_port_, int next_hop_, int distance_) {
    bool found{false};
    if (target_port_ != this->port) {
        for (int i{0}; i < this->vector_size; i++) {
            if (vector_storage[i].target == target_port_) {
                found = true;
                if (distance_ < vector_storage[i].distance) {
                    this->vector_storage[i].next_hop = next_hop_;
                    this->vector_storage[i].distance = distance_;
                    spdlog::debug("Update distance vector entry to for {} to (target={}, next_hop={}, distance={})", target_port_, target_port_, next_hop_,  distance_);
                } else if (distance_ == INT16_MAX && next_hop_ == this->vector_storage[i].next_hop) {
                    this->vector_storage[i].next_hop = next_hop_;
                    this->vector_storage[i].distance = distance_;
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
}

void DistanceVector::parse_vector_update(int source_port_, proto_messages::VectorUpdate update_) {
    spdlog::info("Parsing vector update.", this->port);
    lock_guard<mutex> lg(this->update_mutex);
    for (int i{0}; i < update_.vector_size(); i++) {
        proto_messages::VectorUpdate_VectorEntry v_entry{update_.vector(i)};
        add_or_update_entry(v_entry.target(), source_port_, v_entry.distance() + 1);
    }
    print_debug();
}

void DistanceVector::init() {
    for (size_t i{0}; i < this->neighbours.size(); i++) {
        add_or_update_entry(this->neighbours[i], this->neighbours[i], 1);
    };
    add_or_update_entry(this->port, this->port, 0);
}

int DistanceVector::get_next_hop(int target_) {
    int next_hop{0};
    int shortest_distance{INT16_MAX};
    for (int i{0}; i < vector_size; i++) {
        if (this->vector_storage[i].target == target_ && this->vector_storage[i].distance < shortest_distance) {
            next_hop = this->vector_storage[i].next_hop;
            shortest_distance = this->vector_storage[i].distance;
        }
    }
    return next_hop;
}

void DistanceVector::set_error_distance(int target_port_) {
    for (int i{0}; i < vector_size; i++) {
        if (this->vector_storage[i].target == target_port_) {
            this->vector_storage[i].distance = INT16_MAX;
        }
    }
}

bool DistanceVector::is_reachable(int target_port_) {
    for (int i{0}; i < this->vector_size; i++) {
        if (this->vector_storage[i].target == target_port_ && vector_storage[i].distance == INT16_MAX) {
            spdlog::info("Target {} is set as not reachable, sending will be aborted.", target_port_);
            fmt::print("[{}] Target {} is set as not reachable.\n", format(fg(fmt::color::dark_red), "Node " + to_string(this->port)), target_port_);
            return false;
        }
    }
    return true;
}

void DistanceVector::print_debug() {
    stringstream debug_output_strm;
    for (int i{0}; i < vector_size; i++) {
        debug_output_strm << "\n (target=" << this->vector_storage[i].target << ", next_hop=" << this->vector_storage[i].next_hop << ", distance=" << this->vector_storage[i].distance << ")";
    }
    spdlog::debug(debug_output_strm.str());
}

bool DistanceVector::check_for_failure(int port_) {
    lock_guard<mutex> lg{fail_check_mtx};
    if (port_ == this->failed_connection && this->start_failure) {
        return false;
    }
    return true;
}

void DistanceVector::start_fail() {
    lock_guard<mutex> lg{this->fail_check_mtx};
    this->start_failure = true;
}