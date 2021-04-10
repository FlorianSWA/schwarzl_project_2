/*
Author: Florian Schwarzl
Date: 2021-03-05
*/

#pragma once

// creates network graph with given edges and nodes
std::vector<std::vector<int>> generate_network_graph(int edge_cnt, int node_cnt);

struct NodeData {
    int id;
    std::string port;
    std::vector<char*> cmd_args;
    bool failure{false};
    pid_t pid;
    std::string failed_connection;
};