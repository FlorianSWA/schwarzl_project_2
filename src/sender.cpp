/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#include <iostream>
#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "sender.h"
#include "spdlog/spdlog.h"


using namespace std;

bool Sender::send(string message, string port) {
    asio::ip::tcp::iostream strm{"localhost", port};
    if (strm) {
        spdlog::info("Connected successfully.");
        strm << message << "\n";
        fmt::print("Node {} recieved message: {}\n", 1, message);
        strm.close();
    } else {
        logger->error("Error occured while connecting: {}", strm.error().message());
        return false;
    }
    return true;
}
