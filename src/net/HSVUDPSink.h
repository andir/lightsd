#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <thread>

#include "../Buffer.h"
#include "../hsv.h"

#include <shared_mutex>

class HSVUDPSink {
  using udp = boost::asio::ip::udp;

  boost::asio::io_service io_service;
  udp::socket socket_;

  udp::endpoint remote_endpoint;
  boost::array<HSV, 4096> recv_buffer;

  std::shared_ptr<AbstractBaseBuffer<HSV>> last_frame;

  std::thread worker_thread;
  std::shared_mutex frame_guard;
  const int port;

 public:
  explicit HSVUDPSink(const int port);

  ~HSVUDPSink();

  std::shared_ptr<AbstractBaseBuffer<HSV>> get();

  void start();

  void stop();
  void clear();

 private:
  void recv();

  void run();

  void handle_receive(const boost::system::error_code& error,
                      std::size_t bytes);
};
