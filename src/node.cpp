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
    asio::io_context ctxt;
    tcp::endpoint ep = tcp::endpoint(tcp::v4(), port);
    tcp::acceptor acceptor{ctxt, ep};
    acceptor.listen();
    while (true) {
        tcp::socket sock{ctxt};
        acceptor.accept(sock);
        thread thd(&Node::serve_request, this, move(sock));
        thd.detach();
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