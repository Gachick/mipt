#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

enum LogStatus { LOG_NORMAL, LOG_WARNING, LOG_ERROR };
static constexpr std::string_view LogStatusStr[] = {
    "\033[1;37mINFO\033[0m",
    "\033[1;33mWARNING\033[0m",
    "\033[1;31mERROR\033[0m",
};

struct Log {
  Log(const Log &) = delete;
  void operator*(const Log &) = delete;

  static Log *Instance() {
    if (logInstance == nullptr)
      logInstance = new Log();
    return logInstance;
  }

  void message(LogStatus status, std::string text) {
    if (data.size() == 10) {
      data.pop_back();
    }
    using clock = std::chrono::system_clock;
    auto curTime = clock::to_time_t(clock::now());
    logData message = {text, curTime, status};
    data.insert(data.begin(), message);
  }
  void print() const {
    for (auto dataPiece : data)
      dataPiece.print();
  }

private:
  Log() { std::cout << "Logger initialized" << "\n"; };
  static Log *logInstance;

  struct logData {
    std::string message;
    std::time_t time;
    LogStatus status;
    void print() {
      std::cout << std::ctime(&time) << LogStatusStr[status] << " : " << message
                << "\n";
    }
  };

  std::vector<logData> data;
};

Log *Log::logInstance = nullptr;

int main(void) {
  Log *log = Log::Instance();
  log->message(LOG_NORMAL, "normal loaded");
  log->message(LOG_WARNING, "some warning");
  log->message(LOG_ERROR, "error happens! help me!");
  log->print();
  return 0;
}
