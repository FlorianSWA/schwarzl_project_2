/*
Author: Florian Schwarzl
Date: 2021-02-01
*/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <random>
#include <chrono>
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/fmt/bundled/color.h"
#include "CLI11.hpp"
#include "utils.h"
#include "proto_messages.pb.h"
#include "json.hpp"


using namespace std;


vector<NodeData> nodes;

void sigint_handler(int signal_number) {
    for (size_t i{0}; i < nodes.size(); i++) {
        spdlog::debug("Kill node process {} because of signal {}.", nodes[i].port, signal_number);
        kill(nodes[i].pid, SIGTERM);
    }
    while ((wait(nullptr)) > 0);
    fmt::print("[{}] All subprocesses terminated successfully.\n", format(fg(fmt::color::magenta), "Controller"));
    _exit(103);
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLI::App app("Simulate the distributed algorithm for synchronization");

    bool use_logging{false};
    bool log_level_debug{false};
    bool simulate_error{false};
    size_t node_cnt{7};
    string config_file{"N/A"};

    const char node_program[7]{"./node"};

    app.add_option("node count", node_cnt, "Total number of nodes. Must be > 0.")->check(CLI::PositiveNumber);
    CLI::Option* log_flag{app.add_flag("-l, --log", use_logging, "Write log file dist_sync_log.log.")};
    app.add_flag("-d, --debug", log_level_debug, "Set log level to debug.")->needs(log_flag);
    app.add_option("-f, --file", config_file, "Path to json config file.")->check(CLI::ExistingFile);
    app.add_flag("--failure", simulate_error, "Simulate failure of one connection");

    CLI11_PARSE(app, argc, argv);

    if (config_file != "N/A") {
        ifstream config_ifstream(config_file);
        try {
            nlohmann::json json_config;
            config_ifstream >> json_config;

            if (json_config.contains("nodes")) {
                if (json_config["nodes"].is_number_unsigned()) {
                    node_cnt = json_config["nodes"];
                } else {
                    spdlog::error("Wrong parameter type for nodes, using default value. Expected unsigned integer.");
                }
            } else {
                spdlog::info("Missing parameter node count. Continuing with default values.");
            }
            if (json_config.contains("log")) {
                if (json_config["log"].is_boolean()) {
                    use_logging = json_config["log"];
                    if (json_config.contains("debug")) {
                        if (json_config["debug"].is_boolean()) {
                            log_level_debug = json_config["debug"];
                        } else {
                            spdlog::error("Wrong parameter type for debug, using default value. Expected boolean.");
                        }
                    }
                } else {
                    spdlog::error("Wrong parameter type for log, using default value. Expected boolean.");
                }
            }
            if (json_config.contains("failure")) {
                if (json_config["failure"].is_number_unsigned()) {
                    simulate_error = json_config["failure"];
                } else {
                    spdlog::error("Wrong parameter type for failure, using default value. Expected boolean.");
                }
            }
        } catch (const nlohmann::detail::parse_error &json_err) {
            spdlog::error("{}. Continuing with default values.", json_err.what());
        }
        config_ifstream.close();
    }

    // set logging settings
    if (use_logging) {
        shared_ptr<spdlog::logger> file_logger = spdlog::rotating_logger_mt("file_logger", "./controller.log", 1048576 * 5, 2);
        if (log_level_debug) {
            file_logger->set_level(spdlog::level::debug);
            file_logger->flush_on(spdlog::level::debug);
        } else {
            file_logger->set_level(spdlog::level::info);
            file_logger->flush_on(spdlog::level::info);
        }
        spdlog::set_default_logger(file_logger);
    } else {
        spdlog::default_logger()->set_level(spdlog::level::off);
        spdlog::default_logger()->flush_on(spdlog::level::off);
    }

    unsigned int vertice_cnt = node_cnt + 2;

    vector<vector<int>> network{generate_network_graph(vertice_cnt, node_cnt)};

    for(unsigned long int j{0}; j < node_cnt; j++) {
        NodeData new_node;
        new_node.id = j;
        new_node.port = "990" + to_string(j);
        nodes.push_back(new_node);
    }

    size_t failure_node_1;
    size_t failure_node_2;
    if (simulate_error) {
        random_device rd{};
        mt19937 fail_node_gen{rd()};
        uniform_int_distribution<unsigned long int> fail_node_dis{0, node_cnt - 1ul};
        
        failure_node_1 = fail_node_dis(fail_node_gen);
        failure_node_2 = network[failure_node_1][0];

        nodes[failure_node_1].failure = true;
        nodes[failure_node_1].failed_connection = nodes[failure_node_2].port;

        nodes[failure_node_2].failure = true;
        nodes[failure_node_2].failed_connection = nodes[failure_node_1].port;

        spdlog::info("Connection between node {} and node {}", nodes[failure_node_1].port, nodes[failure_node_2].port);
        spdlog::debug("Failing node 1: {} = {}", nodes[failure_node_1].failed_connection, nodes[failure_node_2].port);
        spdlog::debug("Failing node 2: {} = {}", nodes[failure_node_2].failed_connection, nodes[failure_node_1].port);
    }

    // debug output of network graph
    ostringstream debug_msg;
    debug_msg << "\n";
    for (size_t i{0}; i < network.size(); i++) {
        debug_msg << "[" << nodes[i].port << "] = [";
        for (size_t j{0}; j < network[i].size(); j++) {
           debug_msg << "(ID: " << network[i][j] << ", port: " << nodes[network[i][j]].port << ") ";
        }
        debug_msg  << "]\n";
    }
    spdlog::debug(debug_msg.str());
    
    // add parameters and start node in loop
    for (size_t i{0}; i < node_cnt; i++) {
        vector<char*> node_cmd_args;

        node_cmd_args.push_back((char*)"./node");

        if (use_logging) {
            node_cmd_args.push_back((char*)"--log");
            if (log_level_debug) {
                node_cmd_args.push_back((char*)"--debug");
            }
        }

        node_cmd_args.push_back((char*)"-p");
        node_cmd_args.push_back(&nodes[i].port[0]);

        node_cmd_args.push_back((char*)"-n");
        for (size_t j{0}; j < network[i].size(); j++) {
            //spdlog::debug("Next node port {}", network[i][j]);
            node_cmd_args.push_back(&nodes[(network[i][j])].port[0]);
        }

        if (nodes[i].failure) {
            node_cmd_args.push_back((char*)"--failure");
            node_cmd_args.push_back(&nodes[i].failed_connection[0]);
        }

        node_cmd_args.push_back(NULL);
        nodes[i].cmd_args = node_cmd_args;

        nodes[i].pid = fork();
        if (nodes[i].pid == -1) {
            spdlog::error("Creating node process {} failed.", i);
        } else if (nodes[i].pid> 0) {
            fmt::print("[{}] Create node process with pid {}.\n", format(fg(fmt::color::magenta), "Controller"), nodes[i].pid);
            spdlog::info("Create node process with pid {}.", nodes[i].pid);
        } else {
            char** node_argv{node_cmd_args.data()};
            execv(node_program, &node_argv[0]);
            perror("execl");
            exit(EXIT_FAILURE);
        }
    }
    signal(SIGINT, sigint_handler);

    pid_t node_pid;
    while ((node_pid = wait(nullptr)) > 0) {
        fmt::print("[{}] Node process {} terminated.\n", format(fg(fmt::color::magenta), "Controller"), node_pid);
    }
    return 0;
}
