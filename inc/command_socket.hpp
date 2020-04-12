#ifndef COMMANDSOCKET_HPP
#define COMMANDSOCKET_HPP

#include <chrono>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "base_socket.hpp"
#include "joystick.hpp"
class CommandSocket : public BaseSocket {
public:

  CommandSocket(asio::io_service& io_service, const std::string& drone_ip, const std::string& drone_port, const std::string& local_port, int n_retries_allowed = 1, int timeout = 7);
  void executeQueue();
  void addCommandToQueue(const std::string& cmd);
  void addCommandToFrontOfQueue(const std::string& cmd);
  void clearQueue();
  void stopQueueExecution();
  void removeNextFromQueue();
  void doNotAutoLand();
  void allowAutoLand();
  void emergency();
  void stop();
  bool isExecutingQueue();
  void land();
  ~CommandSocket();

private:

  void handleResponseFromDrone(const std::error_code& error, size_t bytes_recvd) override;
  void handleSendCommand(const std::error_code& error, size_t bytes_sent, std::string cmd) override;

  void waitForResponse();
  void retry(const std::string& cmd);
  void sendQueueCommands();
  void sendCommand(const std::string& cmd);
  void doNotAutoLandWorker();

  enum{ max_length_ = 1024 };
  bool waiting_for_response_ = false, execute_queue_ = false, dnal_ = false, on_ = true;
  char data_[max_length_];
  int timeout_, n_retries_ = 0, n_retries_allowed_ = 0, dnal_timeout = 7 /*dnal --> do not auto land*/ ;
  std::string last_command_, response_;
  std::deque<std::string> command_queue_;
  std::mutex queue_mutex_, m, dnal_mutex;
  std::condition_variable cv_execute_queue_, cv_dnal_;
  std::chrono::system_clock::time_point command_sent_time_;

  std::thread cmd_thread, dnal_thread;

  friend class Tello;
};

#endif // COMMANDSOCKET_HPP
