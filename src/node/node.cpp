/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#include <thread>
#include <random>
#include <iostream>
#include "node.h"
#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"
#include "proto_messages.pb.h"


using namespace asio::ip;
using namespace std;

void Node::run() {
    fmt::print("[{}] started.\n", format(fg(fmt::color::cyan), "Node " + to_string(this->message_sender.dv.port)));
    spdlog::info("Node {} started.", this->message_sender.dv.port);

    ostringstream neighbour_debug;
    for (size_t i{0}; i < neighbours.size(); i++) {
        neighbour_debug << neighbours[i] << ", ";
    }
    spdlog::debug("Neighbouring nodes are: {}", neighbour_debug.str());

    string msg{"Testing new message from " + to_string(this->message_sender.dv.port)};
    thread sender_thd{ref(message_sender), msg};
    sender_thd.detach();

    asio::io_context ctxt;
    tcp::endpoint ep = tcp::endpoint(tcp::v4(), this->message_sender.dv.port);
    tcp::acceptor acceptor{ctxt, ep};
    acceptor.listen();

    while (true) {
        if (failure) {
            this->fail_cnt++;
            if (this->fail_cnt > 10ul * direct_neighbours) {
                spdlog::info("Node is simulating failure and shutting down.");
                exit(103);
            } 
        }
        tcp::socket sock{ctxt};
        acceptor.accept(sock);
        thread handler{&Node::serve_request, this, move(sock)};
        handler.detach();
    }
}

void Node::serve_request(tcp::socket&& sock) {;
    asio::streambuf buf;
    asio::read_until(sock, buf, '\n');
    proto_messages::WrapperMessage message;
    istream is{&buf};
    message.ParseFromIstream(&is);
    if (message.target() == this->message_sender.dv.port) {
        if (message.has_text_message()) {
            fmt::print("[{}] Received: " + message.text_message().text() + "\n", format(fg(fmt::color::cyan), "Node " + to_string(this->message_sender.dv.port)));
            spdlog::info("Received message: {}", this->message_sender.dv.port, message.text_message().text());
        } else if (message.has_update_message()) {
            this->message_sender.dv.parse_vector_update(message.source(), message.update_message());
        }
    } else {
        this->message_sender.redirect(message);
    }

    sock.close();
}
