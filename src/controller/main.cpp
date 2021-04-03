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
#include "proto_messages.pb.h"
#include "json.hpp"


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
    size_t node_cnt{5};
    string config_file{"N/A"};

    const char node_program[7]{"./node"};

    app.add_option("node count", node_cnt, "Total number of nodes.");
    CLI::Option* log_flag{app.add_flag("-l, --log", use_logging, "Write log file dist_sync_log.log.")};
    app.add_flag("-d, --debug", log_level_debug, "Set log level to debug.")->needs(log_flag);
    app.add_option("-f, --file", config_file, "Path to json config file.")->check(CLI::ExistingFile);

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
                    use_logging = true;
                    if (json_config.contains("debug")) {
                        if (json_config["debug"].is_boolean()) {
                            log_level_debug = true;
                        } else {
                            spdlog::error("Wrong parameter type for debug, using default value. Expected boolean.");
                        }
                    }
                } else {
                    spdlog::error("Wrong parameter type for log, using default value. Expected boolean.");
                }
            }
        } catch (const nlohmann::detail::parse_error &json_err) {
            spdlog::error("{}. Continuing with default values.", json_err.what());
        }
    }

    shared_ptr<spdlog::logger> file_logger = spdlog::rotating_logger_mt("file_logger", "./controller.log", 1048576 * 5, 2);

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

    unsigned int vertice_cnt = node_cnt * 2 - 1;
    if (vertice_cnt < node_cnt) {
        vertice_cnt = node_cnt;
    }
    vector<vector<int>> network{generate_network_graph(vertice_cnt, node_cnt)};

    vector<string> port_list;

    for(size_t i{0}; i < node_cnt; i++) {
        port_list.push_back(to_string(9900 + i));
    }

    ostringstream debug_msg;
    debug_msg << "\n";
    for (size_t i{0}; i < network.size(); i++) {
        debug_msg << "[" << port_list[i] << "] = [";
        for (size_t j{0}; j < network[i].size(); j++) {
           debug_msg << "(ID: " << network[i][j] << ", port: " << port_list[network[i][j]] << ") ";
        }
        debug_msg  << "]\n";
    }
    spdlog::debug(debug_msg.str());
    
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
        node_cmd_args.push_back(&port_list[i][0]);

        node_cmd_args.push_back((char*)"-n");
        for (size_t j{0}; j < network[i].size(); j++) {
            //spdlog::debug("Next node port {}", network[i][j]);
            node_cmd_args.push_back(&port_list[(network[i][j])][0]);
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
