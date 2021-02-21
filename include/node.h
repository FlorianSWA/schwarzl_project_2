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
    int id;
    int port{9999};
    std::vector<int> neighbours;
    std::shared_ptr<spdlog::logger> logger;

    void send_message(std::string message, int interval);

  public:
    Node(int id_, int port_, std::vector<int> neighbours_) {
        id = id_;
        port = port_;
        neighbours = neighbours_;
        logger = spdlog::get("file_logger");
    };
        
    void serve_request(asio::ip::tcp::socket&& sock);

    void operator()();
};
