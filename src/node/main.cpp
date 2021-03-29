/*
Author: Florian Schwarzl
Date: 2021-03-03
*/

#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "CLI11.hpp"
#include "node.h"
#include "proto_messages.pb.h"


using namespace std;

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLI::App app("Simulate the distributed algorithm for synchronization");

    int port{9900};
    vector<int> neighbours;
    vector<int> nodes;
    bool use_logging{false};
    bool log_level_debug{false};

    app.add_option("-p, --port", port, "The port of this node.")->required();
    app.add_option("-n, --neighbours", neighbours, "Vector of neighbouring node ports.");
    app.add_option("-a, --all_nodes", nodes, "Vector of all node ports.");
    CLI::Option* log_flag{app.add_flag("-l, --log", use_logging, "Write log file node_<port>.log.")};
    app.add_flag("-d, --debug", log_level_debug, "Set log level to debug.")->needs(log_flag);

    CLI11_PARSE(app, argc, argv);

    shared_ptr<spdlog::logger> file_logger = spdlog::rotating_logger_mt("file_logger", "./node_" + to_string(port) + ".log", 1048576 * 5, 1);

    if (use_logging) {
        if (log_level_debug) {
            file_logger->set_level(spdlog::level::debug);
            file_logger->flush_on(spdlog::level::debug);
        } else {
            file_logger->set_level(spdlog::level::info);
            file_logger->flush_on(spdlog::level::info);
        }
    } else {
        file_logger->set_level(spdlog::level::off);
        file_logger->flush_on(spdlog::level::off);
    }
    spdlog::set_default_logger(file_logger);
    spdlog::debug("Started node process {}.", port);

    Node n1(port, nodes);
    n1.run();

    return 0;
}