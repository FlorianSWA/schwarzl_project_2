/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#include <thread>
#include <random>
#include "node.h"
#include "asio.hpp"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/color.h"



using namespace asio::ip;
using namespace std;

void Node::operator()() {
    random_device rd{};
    mt19937 gen{rd()};
    uniform_int_distribution<int> dis{3, 5};

    string msg{"Hello from Node " + to_string(id)};
    thread sender_thd{&Node::send_message, this, msg, dis(gen)};
    sender_thd.detach();

    asio::io_context ctxt;
    tcp::endpoint ep = tcp::endpoint(tcp::v4(), port);
    tcp::acceptor acceptor{ctxt, ep};
    acceptor.listen();

    while (true) {
        tcp::socket sock{ctxt};
        acceptor.accept(sock);
        thread handler{&Node::serve_request, this, move(sock)};
        handler.detach();
    }
}

void Node::serve_request(tcp::socket&& sock) {
    asio::streambuf buf;
    asio::read_until(sock, buf, '\n');
    string reply;
    istream is{&buf};
    getline(is, reply);
    fmt::print("[{}] Recieved: " + reply + "\n", format(fg(fmt::color::royal_blue), "Node " + to_string(id)));
    sock.close();
}

void Node::send_message(string message, int interval) {
    while (true) {
        for (size_t i{0}; i < neighbours.size(); i++) {
            asio::ip::tcp::iostream strm{"localhost", to_string(neighbours[i])};
            if (strm) {
                logger->info("Opened connection between Node {} (sender) and Node {} (reciever).", id, i);

                strm << message << "\n";
                fmt::print("[{}] Node {} recieved message: {}\n", format(fg(fmt::color::royal_blue), "Node " + to_string(id)), id, message);

                strm.close();
                logger->info("Closed connection between Node {} (sender) and Node {} (reciever).", id, i);
            } else {
                logger->error("Error occured while connecting: {}", strm.error().message());
            }
        }
        this_thread::sleep_for(chrono::seconds(interval));
    }
}
