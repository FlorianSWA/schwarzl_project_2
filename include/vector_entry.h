/*
Author: Florian Schwarzl
Date: 2021-03-21

Description: Class representing a single entry in the distance vector
*/

#pragma once

struct VectorEntry{
    int target;
    int next_hop;
    int distance;
};