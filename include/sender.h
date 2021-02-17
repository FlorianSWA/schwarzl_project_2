/*
Author: Florian Schwarzl
Date: 2021-02-16
*/

class Sender {
  private:
    std::shared_ptr<spdlog::logger> logger;

  public:
    Sender() {
        logger = spdlog::get("file_logger");
    }
    void operator()();
};