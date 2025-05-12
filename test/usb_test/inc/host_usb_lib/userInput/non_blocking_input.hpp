/*
 * File: non_blocking_input.hpp
 * Created Date: 2023-04-10
 * Author: Dennis Liu
 * Contact: <liusx880630@gmail.com>
 *
 * Last Modified: Friday July 14th 2023 10:51:24 am
 *
 * Copyright (c) 2023 None
 *
 * -----
 * HISTORY:
 * Date      	 By	Comments
 * ----------	---
 * ----------------------------------------------------------
 */

#pragma once

#include <spdlog/fmt/fmt.h>

#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <host_usb_lib/parser/ui_parser.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace lra::usb_lib {
class NonBlockingInput {
 public:
  NonBlockingInput()
      : uiparser_([this]() { return PopFirst(); }), exit_flag_(false) {
    if (pipe(pipefd_) == -1) {
      throw std::runtime_error("pipe creation failed");
    }
    input_thread_ = std::thread(&NonBlockingInput::InputLoop, this);
  }

  ~NonBlockingInput() {
    exit_flag_.store(true);

    // notify InputLoop by writing to the pipe
    char buf = '\n';
    ::write(pipefd_[1], &buf, 1);

    if (input_thread_.joinable()) {
      input_thread_.join();
    }

    ::close(pipefd_[0]);
    ::close(pipefd_[1]);
  }

  // XXX: It's for uncomplete CLI interface
  std::string PopFirst() {
    std::unique_lock<std::mutex> lock(input_queue_mutex_);
    input_queue_cv_.wait(
        lock, [this]() { return !input_queue_.empty() || exit_flag_.load(); });

    if (input_queue_.empty()) return {""};
    std::string input = std::move(input_queue_.front());
    input_queue_.pop();
    return input;
  }

  bool InputAvailable() { return !input_queue_.empty(); }

  void RunLoop() {
    while (true) {
      // check should exit
      if (exit_flag_.load()) {
        break;
      }

      std::string input = PopFirst();

      if (input == "e" || input == "exit" || input == "q" || input == "quit") {
        uiparser_.Parse("exit");
        fmt::print(fg(fmt::terminal_color::red), "exit RCWS CLI\n");
        exit_flag_.store(true);
        break;
      } else {
        uiparser_.Parse(input);
      }
    }
  }

  bool GetExitFlag() { return exit_flag_.load(); }

  UIParser uiparser_;

 private:
  void InputLoop() {
    std::string line;
    while (true) {
      fd_set rfds;
      FD_ZERO(&rfds);
      FD_SET(pipefd_[0], &rfds);
      FD_SET(STDIN_FILENO, &rfds);
      int nfds = std::max(pipefd_[0], STDIN_FILENO) + 1;

      // block until stdin or pipe has data
      int ret = select(nfds, &rfds, nullptr, nullptr, nullptr);
      if (ret <= 0) {
        break;  // error or interrupted
      }
      // if pipe has data, that's our exit notification
      if (FD_ISSET(pipefd_[0], &rfds)) {
        break;
      }
      // if stdin has data, read one line
      if (FD_ISSET(STDIN_FILENO, &rfds)) {
        if (!std::getline(std::cin, line)) {
          break;  // EOF or error
        }
        {
          std::lock_guard<std::mutex> lock(input_queue_mutex_);
          input_queue_.push(line);
        }
        input_queue_cv_.notify_one();
      }
    }
    // wake up any waiting PopFirst()
    input_queue_cv_.notify_all();
  }

  int pipefd_[2];
  std::thread input_thread_;
  std::atomic<bool> exit_flag_;
  std::mutex input_queue_mutex_;
  std::condition_variable input_queue_cv_;
  std::queue<std::string> input_queue_;
};
}  // namespace lra::usb_lib