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
#include "CLI11.hpp"


using namespace std;

void sigint_handler(int signal_number) {
    for (size_t i{0}; i < node_processes.size(); i++) {
        kill(node_processes[i], SIGTERM);
    }
    while ((wait(nullptr)) > 0);
    _exit(handler_exit_code);
}

vector<pid_t> node_processes;

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

    vector<int> nodes;
    int port_list[node_cnt];

    for(int i{0}; i < node_cnt; i++) {
        port_list[i] = 9900 + i;
    }

    const char node_program[7]{"./node"};
    
    for (size_t i{0}; i < node_cnt; i++) {
        string port_arg;

        for (int i{0}; i < node_cnt; i++) {
            port_arg += to_string(port_list[i]);
        }

        pid_t node_pid{fork()};
        if (node_pid == -1) {
            file_logger->error("Creating child process {} failed.", i);
        } else if (node_pid > 0) {
            fmt::print("Create node process with pid {}.\n", node_pid);
        } else {
            execl(node_program, args, nullptr);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }    
    fmt::print("This is the controller\n");

    pid_t node_pid;
    while ((node_pid = wait(nullptr)) > 0) {
        fmt::print("child {} terminated.\n", node_pid);
    }

    signal(SIGINT, sigint_handler);

    return 0;
}
