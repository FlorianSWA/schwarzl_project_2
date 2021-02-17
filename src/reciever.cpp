/*
Author: Florian Schwarzl
Date: 202102-16

Desc: Class responsible for handling recieved messages
*/

#include "reciever.h"

void Reciever::operator()() {
    asio::io_context ctx;
    tcp::resolver resolver{ctx};
    try {
        auto results = resolver.resolve("localhost", port);
        tcp::socket sock{ctx};

        asio::connect(sock, results);
        logger
}