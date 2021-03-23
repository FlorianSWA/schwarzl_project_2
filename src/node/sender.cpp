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
#include "distance_vector.pb.h"


using namespace std;

void Sender::send(string message, int port, int sender_port) {
    asio::ip::tcp::iostream strm{"localhost", to_string(port)};
    if (strm) {
        spdlog::debug("Opened connection between Node {} (sender) and Node {} (reciever).", sender_port, port);
        
        proto_messages::SimpleMessage testMessage;
        testMessage.set_text(message);
        testMessage.set_source(sender_port);
        testMessage.set_target(port);

        unsigned long length{testMessage.ByteSizeLong()};
        char* message_array = new char[length];
        testMessage.SerializeToArray(message_array, length);

        strm.write(message_array, length);
        fmt::print("[{}] sent message: {}\n", format(fg(fmt::color::cyan), "Node " + to_string(sender_port)), message);
        strm.close();
        spdlog::debug("Closed connection between Node {} (sender) and Node {} (reciever).", sender_port, port);
        delete message_array;
    } else {
        spdlog::error("Error occured while connecting: {}", strm.error().message());
    }
}

void Sender::operator()(string message) {
    random_device rd{};
    mt19937 gen{rd()};
    uniform_int_distribution<int> dis{3, 5};
    uniform_int_distribution<int> node_dis{0, node_cnt};
    while (true) {
        this_thread::sleep_for(chrono::seconds(dis(gen)));
        send(message, nodes[node_dis(gen)], sender_port);
    }
}
