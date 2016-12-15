#include <mutex>
#include <shared_mutex>
#include "HSVUDPSink.h"


using boost::asio::ip::udp;

HSVUDPSink::HSVUDPSink(const int port) :
        io_service(),
        socket_(io_service, udp::endpoint(udp::v4(), port)) {

    recv();
}


HSVUDPSink::~HSVUDPSink() {
    stop();
}

void HSVUDPSink::handle_receive(const boost::system::error_code &error, std::size_t bytes) {

    if (bytes > 0 && !error) {
        // if this turns out to be super slow, we might just reuse the old buffer
        // and take a lock... let people way for a few ms when their UDP crap is slow
        auto buf = std::make_shared<AllocatedBuffer<HSV>>(bytes / sizeof(HSV));
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


void HSVUDPSink::recv() {
    socket_.async_receive_from(
            boost::asio::buffer(recv_buffer), remote_endpoint,
            boost::bind(&HSVUDPSink::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void HSVUDPSink::run() {
    io_service.run();
}

void HSVUDPSink::start() {
    if (worker_thread.joinable()) {
        if (!io_service.stopped())
            io_service.stop();
        worker_thread.join();
    }
    worker_thread = std::thread(std::bind(&HSVUDPSink::run, this));
}

void HSVUDPSink::stop() {
    if (!io_service.stopped())
        io_service.stop();
    worker_thread.join();
}


std::shared_ptr<AbstractBaseBuffer<HSV>> HSVUDPSink::get() {
    std::shared_lock<std::shared_mutex> guard(frame_guard);

    return last_frame;
}