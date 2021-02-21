/*
Author: Florian Schwarzl
Date: 2021-02-01
*/

#include <iostream>
#include <thread>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "CLI11.hpp"
#include "node.h"


using namespace std;

int main(int argc, char* argv[]) {

    CLI::App app("Simulate the distributed algorithm for synchronization");

    bool use_logging{false};
    bool log_level_debug{false};

    auto log_flag{app.add_flag("-l, --log", use_logging, "Write log file dist_sync_log.log")};
    app.add_flag("-d, --debug", log_level_debug, "Set log level to debug.")->needs(log_flag);

    CLI11_PARSE(app, argc, argv);

    shared_ptr<spdlog::logger> file_logger = spdlog::rotating_logger_mt("file_logger", "./distance_vector.log", 1048576 * 5, 3);

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

    size_t node_cnt{3};
    vector<thread> node_thread_pool;
    vector<int> neighbours{9991, 9992, 9993};

    for (size_t i{0}; i < node_cnt; i++) {
        node_thread_pool.push_back(thread(Node(i, neighbours[i], neighbours)));
    }

    for (size_t i{0}; i < node_cnt; i++) {
        node_thread_pool[i].join();
    }
    
    return 0;
}
