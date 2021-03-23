/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#pragma once

#include <string>
#include <vector>
#include "sender.h"
#include "asio.hpp"


class Node {

  private:
    // Port number of this node
    int port{9999};

    // vector with the port numbers of all nodes
    std::vector<int> neighbours;

    // Handler for message sending
    Sender message_sender;

    // handle connection request on port
    void serve_request(asio::ip::tcp::socket&& sock);

  public:
    Node(int port_, std::vector<int> neighbours_): message_sender(port_, neighbours_) {
        port = port_;
        neighbours = neighbours_;
    };

    void run();
};
