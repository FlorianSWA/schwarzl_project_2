/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#include <string>
#include <vector>

class Node {

  private:
    int id;
    std::string port;
    std::vector<std::string> neighbours;

  public:
    Node(int id_, std::string port_, std::vector<std::string> neighbours_) {
        id = id_;
        port = port_;
        neighbours = neighbours_;
    };

    void operator()();
};
