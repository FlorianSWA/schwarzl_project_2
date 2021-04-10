/*
Author: Florian Schwarzl
Date: 2021-03-03
*/

#include <stdlib.h>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "CLI11.hpp"
#include "node.h"
#include "proto_messages.pb.h"
#include "json.hpp"


using namespace std;

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLI::App app("Simulation of a node in a network.");

    int port{9900};
    vector<int> neighbours;
    bool use_logging{false};
    bool log_level_debug{false};
    int failure{0};
    string config_file{"N/A"};

    app.add_option("-p, --port", port, "The port of this node.")->required();
    app.add_option("-n, --neighbours", neighbours, "Vector of neighbouring node ports.");
    CLI::Option* log_flag{app.add_flag("-l, --log", use_logging, "Write log file node_<port>.log.")};
    app.add_flag("-d, --debug", log_level_debug, "Set log level to debug.")->needs(log_flag);
    app.add_option("-f, --file", config_file, "Path to JSON config file.")->check(CLI::ExistingFile);
    app.add_option("--failure", failure, "Simulate failure of one random connection");

    CLI11_PARSE(app, argc, argv);

    if (config_file != "N/A") {
        ifstream config_ifstream(config_file);
        try {
            nlohmann::json json_config;
            config_ifstream >> json_config;

            if (json_config.contains("port")) {
                if (json_config["port"].is_number_unsigned()) {
                    port = json_config["port"];
                } else {
                    spdlog::error("Wrong parameter type for port, using default value. Expected unsigned integer.");
                }
            } else {
                spdlog::info("Missing parameter port. Continuing with default values.");
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
            if (json_config.contains("failure")) {
                if (json_config["failure"].is_number_unsigned()) {
                    failure = json_config["failure"];
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
        shared_ptr<spdlog::logger> file_logger = spdlog::rotating_logger_mt("file_logger", "./node_" + to_string(port) + ".log", 1048576 * 5, 1);
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
    spdlog::info("Started node process {}.", port);

    Node n1(port, neighbours, failure);
    n1.run();

    return 0;
}