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

    // spdlog file logger
    std::shared_ptr<spdlog::logger> logger;

    // Handler for message sending
    Sender message_sender;

    // send message to all nodes (including self)
    void broadcast_message(std::string message, int interval);

    // send message to specified port
    void send_to(int port, std::string message);

    // handle connection request on port
    void serve_request(asio::ip::tcp::socket&& sock);

  public:
    Node(int port_, std::vector<int> neighbours_): message_sender(port_, neighbours_) {
        port = port_;
        neighbours = neighbours_;
        logger = spdlog::get("file_logger");
    };

    void run();
};
