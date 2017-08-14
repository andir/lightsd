#include <mutex>
#include <shared_mutex>
#include "RGBUDPSink.h"


using boost::asio::ip::udp;

RGBUDPSink::RGBUDPSink(const int port) :
        io_service(),
        socket_(io_service, udp::endpoint(udp::v4(), port)) {

    recv();
}


RGBUDPSink::~RGBUDPSink() {
    stop();
}

void RGBUDPSink::handle_receive(const boost::system::error_code &error, std::size_t bytes) {

    if (bytes > 0 && !error) {
        // if this turns out to be super slow, we might just reuse the old buffer
        // and take a lock... let people way for a few ms when their UDP crap is slow
        auto buf = std::make_shared<AllocatedBuffer<RGB>>(bytes / sizeof(RGB));
        for (size_t i = 0; i < buf->size(); i++) {
            buf->at(i) = recv_buffer[i];
        }
        std::lock_guard<std::shared_mutex> guard(frame_guard);
        last_frame = std::move(buf);
    } else {
        throw error;
    }

    recv();
}


void RGBUDPSink::recv() {
    socket_.async_receive_from(
            boost::asio::buffer(recv_buffer), remote_endpoint,
            boost::bind(&RGBUDPSink::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void RGBUDPSink::run() {
    io_service.run();
}

void RGBUDPSink::start() {
    if (worker_thread.joinable()) {
        if (!io_service.stopped())
            io_service.stop();
        worker_thread.join();
    }
    worker_thread = std::thread(std::bind(&RGBUDPSink::run, this));
}

void RGBUDPSink::stop() {
    if (!io_service.stopped())
        io_service.stop();
    worker_thread.join();
}


std::shared_ptr<AbstractBaseBuffer<RGB>> RGBUDPSink::get() {
    std::shared_lock<std::shared_mutex> guard(frame_guard);

    return last_frame;
}