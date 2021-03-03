/*
Author: Florian Schwarzl
Date: 2021-03-03
*/

#include "spdlog/fmt/fmt.h"
#include "CLI11.hpp"
#include "node.h"


using namespace std;

int main(int argc, char* argv[]) {
    CLI::App app("Simulate the distributed algorithm for synchronization");

    int port{9900};
    vector<int> neighbours;

    app.add_option("port", port, "The port of this node.");
    app.add_option("neighbours", neighbours, "Vector of all nodes.");

    CLI11_PARSE(app, argc, argv);

    fmt::print("Hello from the other side.\n");
    Node n1(port);
    n1();
    return 0;
}