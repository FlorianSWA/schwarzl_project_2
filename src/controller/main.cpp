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
#include "utils.h"
#include "distance_vector.pb.h"


using namespace std;


vector<pid_t> node_process_pids;

void sigint_handler(int signal_number) {
    for (size_t i{0}; i < node_process_pids.size(); i++) {
        spdlog::debug("Kill process {} with signal {}.", node_process_pids[i], signal_number);
        kill(node_process_pids[i], SIGTERM);
    }
    while ((wait(nullptr)) > 0);
    _exit(103);
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLI::App app("Simulate the distributed algorithm for synchronization");

    bool use_logging{false};
    bool log_level_debug{false};
    size_t node_cnt{4};
    string config_file;

    const char node_program[7]{"./node"};

    app.add_option("node count", node_cnt, "Total number of nodes.");
    CLI::Option* log_flag{app.add_flag("-l, --log", use_logging, "Write log file dist_sync_log.log.")};
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
    spdlog::set_default_logger(file_logger);

    vector<vector<string>> network{generate_network_graph(node_cnt * 2 - 1, node_cnt)};

    ostringstream debug_msg;
    for (size_t i{0}; i < network.size(); i++) {
        debug_msg << "Vector[" << i << "] = [";
        for (size_t j{0}; j < network[i].size(); j++) {
           debug_msg << network[i][j];
        }
        debug_msg  << "]\n";
    }
    spdlog::debug(debug_msg.str());

    vector<string> port_list;

    for(size_t i{0}; i < node_cnt; i++) {
        port_list.push_back(to_string(9900 + i));
    }
    
    for (size_t i{0}; i < node_cnt; i++) {
        vector<char*> node_cmd_args;
        node_cmd_args.push_back(const_cast<char*>("./node"));

        if (use_logging) {
            node_cmd_args.push_back(const_cast<char*>("--log"));
            if (log_level_debug) {
                node_cmd_args.push_back(const_cast<char*>("--debug"));
            }
        }
        node_cmd_args.push_back(const_cast<char*>("-p"));
        node_cmd_args.push_back(const_cast<char*>(port_list[i].c_str()));

        node_cmd_args.push_back(const_cast<char*>("-a"));
        for (size_t j{0}; j < node_cnt; j++) {
            if (port_list[j] != port_list[i]) {
                node_cmd_args.push_back(const_cast<char*>(port_list[j].c_str()));
            }
        }
        node_cmd_args.push_back(const_cast<char*>("-n"));
        for (size_t j{0}; j < network[i].size(); j++) {
            node_cmd_args.push_back(const_cast<char*>(network[i][j].c_str()));
        }

        node_cmd_args.push_back(NULL);

        pid_t node_pid{fork()};
        if (node_pid == -1) {
            spdlog::error("Creating node process {} failed.", i);
        } else if (node_pid > 0) {
            fmt::print("[{}] Create node process with pid {}.\n", format(fg(fmt::color::magenta), "Controller"), node_pid);
            spdlog::info("Create node process with pid {}.", node_pid);
        } else {
            char** node_argv{node_cmd_args.data()};
            execv(node_program, &node_argv[0]);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }    

    pid_t node_pid;
    while ((node_pid = wait(nullptr)) > 0) {
        fmt::print("[{}] Node process {} terminated.\n", format(fg(fmt::color::magenta), "Controller"), node_pid);
    }

    signal(SIGINT, sigint_handler);

    return 0;
}
