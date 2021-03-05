/*
Author: Florian Schwarzl
Date: 2021-03-05
*/

#include "spdlog/fmt/fmt.h"
#include "utils.h"

using namespace std;


void GenerateNetworkGraph(int edge_cnt, int node_cnt) {
    int i {0};
    vector<vector<int>> edge;
    edge.reserve(edge_cnt);
    //Assign random values to the number of vertex and edges of the graph, Using rand().
    while (i < edge_cnt) {
        edge[i].push_back(rand() % node_cnt + 1);
        edge[i].push_back(rand() % node_cnt + 1);
        //Print the connections of each vertex, irrespective of the direction.
        if (edge[i][0] == edge[i][1]) {
            continue;
        } else {
            for (int j{0}; j < i; j++) {
                if ((edge[i][0] == edge[j][0] && edge[i][1] == edge[j][1]) || (edge[i][0] == edge[j][1] && edge[i][1] == edge[j][0])) {
                    i--;
                }
            }
      }
      i++;
   }

    for (int i{0}; i < node_cnt; i++){
        int count{0};
        fmt::print("\n\t {} -> {{ ", i + 1);
        for(int j{0}; j < edge_cnt; j++) {
            if (edge[j][0] == i+1) {
                fmt::print("{}", edge[j][1]);
                count++;
            } else if (edge[j][1] == i + 1) {
                fmt::print("{}", edge[j][0]);
                count++;
            } else if (j== edge_cnt-1 && count == 0) {
                fmt::print("Isolated Vertex!"); //cout<<"Isolated Vertex!"; //Print “Isolated vertex” for the vertex having no degree.
            }
      }
      fmt::print("}}");
   }
   fmt::print("\n\n");
}