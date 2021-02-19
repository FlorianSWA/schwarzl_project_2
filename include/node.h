/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#pragma once

#include <string>
#include <vector>
#include "sender.h"


class Node {

  private:
    int id;
    int port{9999};
    std::vector<std::string> neighbours;
    Sender message_sender();
    void serve_request();

  public:
    Node(int id_, int port_, std::vector<std::string> neighbours_) {
        id = id_;
        port = port_;
        neighbours = neighbours_;
    };

    void operator()();
};
