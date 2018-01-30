#pragma once

#include <yaml-cpp/yaml.h>
#include <boost/asio.hpp>
#include "Output.h"

using boost::asio::ip::udp;

class HSVUDPOutput : public Output {
  boost::asio::io_service io_service;
  udp::socket socket;
  udp::resolver resolver;
  udp::endpoint endpoint;

 public:
  explicit HSVUDPOutput(const YAML::Node& params);

  HSVUDPOutput(const std::string& destination, const std::string& port);

  void draw(const std::vector<HSV>& buffer);

  void draw(const AbstractBaseBuffer<HSV>& buffer);

 private:
  template <typename Container>
  void _send(const Container& buffer) {
    std::vector<boost::asio::const_buffer> buffers;

    buffers.push_back(boost::asio::buffer((char*)&*buffer.begin(),
                                          buffer.size() * sizeof(HSV)));

    socket.send_to(buffers, endpoint);
  }
};
