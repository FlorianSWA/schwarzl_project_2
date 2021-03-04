/*
Author: Florian Schwarzl
Date: 2021-02-01
*/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/fmt/bundled/color.h"
#include "CLI11.hpp"


using namespace std;


vector<pid_t> node_processes;

void sigint_handler(int signal_number) {
    for (size_t i{0}; i < node_processes.size(); i++) {
        spdlog::get("file_logger")->debug("Kill process {} with signal {}.", node_processes[i], signal_number);
        kill(node_processes[i], SIGTERM);
    }
    while ((wait(nullptr)) > 0);
    _exit(103);
}

int main(int argc, char* argv[]) {

    CLI::App app("Simulate the distributed algorithm for synchronization");

    bool use_logging{false};
    bool log_level_debug{false};
    size_t node_cnt{4};
    string config_file;

    app.add_option("node count", node_cnt, "Total number of nodes.");
    auto log_flag{app.add_flag("-l, --log", use_logging, "Write log file dist_sync_log.log.")};
    app.add_flag("-d, --debug", log_level_debug, "Set log level to debug.")->needs(log_flag);
    app.add_flag("-f, --file", config_file, "Path to json config file.");

    CLI11_PARSE(app, argc, argv);

    shared_ptr<spdlog::logger> file_logger = spdlog::rotating_logger_mt("file_logger", "./distance_vector.log", 1048576 * 5, 2);

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

    vector<string> port_list;

    for(size_t i{0}; i < node_cnt; i++) {
        port_list.push_back(to_string(9900 + i));
    }

    const char node_program[7]{"./node"};
    
    for (size_t i{0}; i < node_cnt; i++) {
        vector<char*> node_args;
        node_args.push_back(const_cast<char*>(port_list[i].c_str()));

        for (size_t j{0}; j < node_cnt; j++) {
            if (port_list[j] != port_list[i]) {
                node_args.push_back(const_cast<char*>(port_list[i].c_str()));
            }
        }
        if (use_logging) {
            node_args.push_back(const_cast<char*>("-l"));
            if (log_level_debug) {
                node_args.push_back(const_cast<char*>("-d"));
            }
        }

        node_args.push_back(NULL);

        pid_t node_pid{fork()};
        if (node_pid == -1) {
            file_logger->error("Creating node process {} failed.", i);
        } else if (node_pid > 0) {
            fmt::print("[{}] Create node process with pid {}.\n", format(fg(fmt::color::magenta), "Controller"), node_pid);
            file_logger->info("Create node process with pid {}.", node_pid);
        } else {
            execv(node_program, &node_args.data()[0]);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }    

    pid_t node_pid;
    while ((node_pid = wait(nullptr)) > 0) {
        fmt::print("Node process {} terminated.\n", node_pid);
    }

    signal(SIGINT, sigint_handler);

    return 0;
}
