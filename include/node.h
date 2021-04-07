/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#pragma once

#include <string>
#include <vector>
#include "distance_vector.h"
#include "sender.h"
#include "asio.hpp"


class Node {

  private:
    // vector with the port numbers of all nodes
    std::vector<int> neighbours;

    // Handler for message sending
    Sender message_sender;

    // handle connection request on port
    void serve_request(asio::ip::tcp::socket&& sock);

    // sets this node to fail after some time 
    bool failure;

    // counter for failure timer. When set to fail, node will terminatte after 5 * direct_neighbours iterations
    unsigned long fail_cnt{0};

    // number of direct connections
    unsigned long direct_neighbours;

  public:
    Node(int port_, std::vector<int> neighbours_, bool failure_): message_sender(port_, neighbours_) {
        this->neighbours = neighbours_;
        this->failure = failure_;
        this->direct_neighbours = neighbours_.size();
    };

    void run();
};
