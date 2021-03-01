/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#pragma once

#include "spdlog/spdlog.h"

class Sender {
  private:
    // Pointer to rotating file logger
    std::shared_ptr<spdlog::logger> logger;

    // array with the port numbers of neighbouring nodes
    std::vector<int> neighbours;

    // array with port numbers of all nodes
    std::vector<int> nodes;

    int node_cnt;

    int sender_port;

    // Send messsage to specified port
    void send(std::string message, std::string port, int sender_port);

  public:
    Sender(int sender_port_, std::vector<int> nodes_) {
        sender_port = sender_port_;
        nodes = nodes_;
        logger = spdlog::get("file_logger");
        node_cnt = nodes_.size() - 1;
    }
    void operator()(std::string message);
};