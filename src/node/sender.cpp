/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#include <iostream>
#include <random>
#include <thread>
#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"
#include "sender.h"
#include "spdlog/spdlog.h"


using namespace std;

void Sender::send(string message, string port, int sender_port) {
    asio::ip::tcp::iostream strm{"localhost", port};
    if (strm) {
        logger->debug("Opened connection between Node {} (sender) and Node {} (reciever).", sender_port, port);
        strm << message << "\n";
        fmt::print("[{}] sent message: {}\n", format(fg(fmt::color::royal_blue), "Node " + to_string(sender_port)), message);
        strm.close();
        logger->debug("Closed connection between Node {} (sender) and Node {} (reciever).", sender_port, port);
    } else {
        logger->error("Error occured while connecting: {}", strm.error().message());
    }
}

void Sender::operator()(string message) {
    random_device rd{};
    mt19937 gen{rd()};
    uniform_int_distribution<int> dis{3, 5};
    uniform_int_distribution<int> node_dis{0, node_cnt};
    while (true) {
        this_thread::sleep_for(chrono::seconds(dis(gen)));
        send(message, to_string(nodes[node_dis(gen)]), sender_port);
    }
}
