/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#pragma once

#include "distance_vector.h"

class Sender {
  private:
    // total amount of nodes in network
    size_t node_cnt;

    // Send text messsage to specified port
    void send_text(std::string message, int target_port_, int sender_port);

    // sends vector update to all neighbours
    void send_update_to_neighbours();

  public:
    // Distance vector storage structure
    DistanceVector dv;

    Sender(int sender_port_, std::vector<int> nodes_, int failed_conn): dv(sender_port_, nodes_, failed_conn) {
        node_cnt = nodes_.size();
    }
    void operator()(std::string message);

    // routes message to next hop
    void redirect(proto_messages::WrapperMessage message_);

    // send vector update
    void send_update(int target_port_);
};