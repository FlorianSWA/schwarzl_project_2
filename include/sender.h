/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#pragma once

#include "distance_vector.h"

class Sender {
  private:
    // array with the port numbers of neighbouring nodes
    std::vector<int> neighbours;

    // array with port numbers of all nodes
    std::vector<int> nodes;

    int node_cnt;

    int sender_port;

    // Send text messsage to specified port
    void send_text(std::string message, int port, int sender_port);

    // send vector update
    void send_update();

  public:
    // Distance vector storage structure
    DistanceVector dv;

    Sender(int sender_port_, std::vector<int> nodes_): dv(sender_port_) {
        sender_port = sender_port_;
        nodes = nodes_;
        node_cnt = nodes_.size() - 1;
    }
    void operator()(std::string message);

    void redirect(proto_messages::WrapperMessage message_);
};