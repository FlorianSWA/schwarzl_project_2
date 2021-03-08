/*
Author: Florian Schwarzl
Date: 2021-03-05
*/

#include <random>
#include "utils.h"

using namespace std;


vector<vector<int>> generate_network_graph(int edge_cnt, int node_cnt) {
    auto edge = new int[edge_cnt][2];

    vector<vector<int>> returnVec;
    for (int i{0}; i < node_cnt; i++) {
        returnVec.push_back(vector<int>());
    }

    int i{0};
    
    while (i < edge_cnt) {
        edge[i][0] = rand()%node_cnt+1;
        edge[i][1] = rand()%node_cnt+1;

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
            if (edge[j][0] == i + 1) {
                returnVec[i].push_back(edge[j][1]);
                count++;
            } else if (edge[j][1] == i + 1) {
                returnVec[i].push_back(edge[j][0]);
                count++;
            } else if (j== edge_cnt - 1 && count == 0) {

            }
        }
    }
    delete[] edge;
    return returnVec;
}