/*
Author: Florian Schwarzl
Date: 2021-03-05
*/

#include <random>
#include <sys/wait.h>
#include <chrono>
#include "utils.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"

using namespace std;


vector<vector<int>> generate_network_graph(int edge_cnt, int node_cnt) {
    auto edge = new int[edge_cnt][2];
    int i{0};
    random_device rd{};
    mt19937 gen{rd()};
    uniform_int_distribution<int> node_dis{0, node_cnt - 1};

    vector<vector<int>> returnVec;
    for (int i{0}; i < node_cnt; i++) {
        returnVec.push_back(vector<int>());
    }
    
    while (i < edge_cnt) {
        edge[i][0] = node_dis(gen);
        edge[i][1] = node_dis(gen);

        if(edge[i][0] == edge[i][1]) {
            continue;
        }
        else {
            for (int j{0}; j < i; j++) {
                if ((edge[i][0] == edge[j][0] && edge[i][1] == edge[j][1]) || (edge[i][0] == edge[j][1] && edge[i][1] == edge[j][0])) {
                    i--;
                }
            }
        }
        i++;
    }
    for (int i{0}; i < node_cnt; i++) {
        int count{0};
        for (int j{0}; j < edge_cnt; j++) {
            if (edge[j][0] == i) {
                returnVec[i].push_back(edge[j][1]);
                count++;
            } else if (edge[j][1] == i) {
                returnVec[i].push_back(edge[j][0]);
                count++;
            } else if (j == edge_cnt - 1 && count == 0) {
                fmt::print("[{}] Isolated Node created!\n", format(fg(fmt::color::magenta), "Controller"));
                spdlog::error("Network graph creation: Isolated vertex created!");
            }
        }
    }
    delete[] edge;
    return returnVec;
}