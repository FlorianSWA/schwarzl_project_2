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
#include "proto_messages.pb.h"


using namespace std;

void Sender::send_text(string message, int port, int sender_port) {
    asio::ip::tcp::iostream strm{"localhost", to_string(port)};
    if (strm) {
        spdlog::debug("Opened connection between Node {} (sender) and Node {} (reciever).", sender_port, port);
        
        proto_messages::WrapperMessage wrapper;
        wrapper.set_source(sender_port);
        wrapper.set_target(port);

        proto_messages::SimpleMessage* sm = wrapper.mutable_text_message();
        sm->set_text(message);

        strm << wrapper.SerializeAsString();
        strm << "\n";
        fmt::print("[{}] sent message: {}\n", format(fg(fmt::color::cyan), "Node " + to_string(sender_port)), message);
        strm.close();
        spdlog::debug("Closed connection between Node {} (sender) and Node {} (reciever).", sender_port, port);
        //delete message_array;
        //delete sm;
    } else {
        spdlog::error("Error occured while connecting: {}", strm.error().message());
    }
}

void Sender::operator()(string message) {
    dv.init(this->neighbours);
    random_device rd{};
    mt19937 gen{rd()};
    uniform_int_distribution<int> dis{3, 5};
    uniform_int_distribution<int> node_dis{0, dv.vector_size};
    while (true) {
        //this_thread::sleep_for(chrono::seconds(dis(gen)));
        //send_update();
        this_thread::sleep_for(chrono::seconds(dis(gen)));
        send_text(message, dv.vector_storage[node_dis(gen)].target, sender_port);
    }
}

void Sender::redirect(proto_messages::WrapperMessage message_) {
    string next_hop{to_string(this->dv.get_next_hop(message_.target()))};
    asio::ip::tcp::iostream strm{"localhost", next_hop};
    if (strm) {
        spdlog::debug("Opened connection between Node {} (sender) and Node {} (reciever).", sender_port, next_hop);

        strm << message_.SerializeAsString();
        strm << "\n";
        fmt::print("[{}] Redirect message to {}\n", format(fg(fmt::color::cyan), "Node " + to_string(sender_port)), next_hop);
        strm.close();
        spdlog::debug("Closed connection between Node {} (sender) and Node {} (reciever).", sender_port, next_hop);
    } else {
        spdlog::error("Error occured while connecting: {}", strm.error().message());
    }
}

void Sender::send_update() {
}