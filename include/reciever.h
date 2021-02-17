/*
Author: Florian Schwarzl
Date: 2021-02-16

Desc: Class responsible for handling recieved messages
*/

#include "spdlog/spdlog.h"

class Reciever {
  private:
    std::shared_ptr<spdlog::logger> logger;
  
  public:
    Reciever() {
        
    }
    void operator()();
}