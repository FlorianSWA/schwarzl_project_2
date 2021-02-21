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
    // Unique Id of this node
    int id;

    // Port number of this node
    int port{9999};

    // vector with the port numbers of all nodes
    std::vector<int> neighbours;

    // spdlog file logger
    std::shared_ptr<spdlog::logger> logger;

    // send message to all nodes (including self)
    void send_message(std::string message, int interval);

  public:
    Node(int id_, int port_, std::vector<int> neighbours_) {
        id = id_;
        port = port_;
        neighbours = neighbours_;
        logger = spdlog::get("file_logger");
    };
    
    // handle connection request on port
    void serve_request(asio::ip::tcp::socket&& sock);

    void operator()();
};
