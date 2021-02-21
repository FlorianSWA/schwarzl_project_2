/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class representing a node in the simulated network
*/

#include <thread>
#include "node.h"
#include "asio.hpp"
#include "spdlog/fmt/fmt.h"



using namespace asio::ip;
using namespace std;

void Node::operator()() {
    thread sender_thd{&Node::send_message, this, "Hello from Node " + to_string(id), 5};
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
    fmt::print("Recieved: " + reply + "\n");
    sock.close();
}

void Node::send_message(string message, int interval) {
    while (true) {
        for (size_t i{0}; i < neighbours.size(); i++) {
            asio::ip::tcp::iostream strm{"localhost", to_string(neighbours[i])};
            if (strm) {
                spdlog::info("Connected successfully.");
                strm << message << "\n";
                fmt::print("Node {} recieved message: {}\n", id, message);
                strm.close();
            } else {
                logger->error("Error occured while connecting: {}", strm.error().message());
            }
        }
        this_thread::sleep_for(chrono::seconds(interval));
    }
}
