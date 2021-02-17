/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#include <iostream>
#include "asio.hpp"
#include "sender.h"

using namespace std;

void Sender::operator()() {
    asio::io_context ctx;
    tcp::resolver resolver{ctx};
    try {
        auto results = resolver.resolve("localhost", port);
        tcp::socket sock{ctx};

        asio::connect(sock, results);
        logger
}
