#pragma once

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <thread>

#include "../rgb.h"
#include "../Buffer.h"

#include <shared_mutex>

class RGBUDPSink {
    using udp = boost::asio::ip::udp;

    boost::asio::io_service io_service;
    udp::socket socket_;

    udp::endpoint remote_endpoint;
    boost::array<RGB, 4096> recv_buffer;

    std::shared_ptr<AbstractBaseBuffer<RGB>> last_frame;

    std::thread worker_thread;
    std::shared_mutex frame_guard;

public:

    explicit RGBUDPSink(const int port);

    ~RGBUDPSink();

    std::shared_ptr<AbstractBaseBuffer<RGB>> get();

    void start();

    void stop();


private:

    void recv();

    void run();

    void handle_receive(const boost::system::error_code &error, std::size_t bytes);
};
