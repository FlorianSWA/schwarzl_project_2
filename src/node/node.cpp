/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#include <thread>
#include <random>
#include "node.h"
#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"
#include "proto_messages.pb.h"


using namespace asio::ip;
using namespace std;

void Node::run() {
    fmt::print("[{}] started.\n", format(fg(fmt::color::cyan), "Node " + to_string(port)));
    spdlog::info("Node {} started.", port);

    random_device rd{};
    mt19937 gen{rd()};
    uniform_int_distribution<int> dis{6, 9};

    string msg{"Testing new message from " + to_string(port)};
    thread sender_thd{ref(message_sender), msg};
    //thread sender_thd{&Node::broadcast_message, this, msg, dis(gen)};
    sender_thd.detach();

    asio::io_context ctxt;
    tcp::endpoint ep = tcp::endpoint(tcp::v4(), port);
    tcp::acceptor acceptor{ctxt, ep};
    fmt::print("[{}] waiting for connection.\n", format(fg(fmt::color::cyan), "Node " + to_string(port)));
    acceptor.listen();

    while (true) {
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
    if (message.target() == port) {
        if (message.has_text_message()) {
            fmt::print("[{}] Received: " + message.text_message().text() + "\n", format(fg(fmt::color::cyan), "Node " + to_string(port)));
            spdlog::info("Node {} received message: {}", this->port, message.text_message().text());
        } else if (message.has_update_message()) {
            this->message_sender.dv.parse_vector_update(message.source(), message.update_message());
        }
    } else {
        this->message_sender.redirect(message);
    }

    sock.close();
}
