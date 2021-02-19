/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

#pragma once

#include "spdlog/spdlog.h"

class Sender {
  private:
    std::shared_ptr<spdlog::logger> logger;

  public:
    Sender() {
        logger = spdlog::get("file_logger");
    }
    bool send(std::string message, std::string port);
};