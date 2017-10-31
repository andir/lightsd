#include <mutex>
#include <shared_mutex>
#include "HSVUDPSink.h"


using boost::asio::ip::udp;

HSVUDPSink::HSVUDPSink(const int port) :
        io_service(),
        socket_(io_service),
        port(port) {
        socket_.open(udp::v6());
        using reuse_port = boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>;
        socket_.set_option(reuse_port(true));
        socket_.bind(udp::endpoint(udp::v6(), port));
}


HSVUDPSink::~HSVUDPSink() {
    stop();
}

void HSVUDPSink::handle_receive(const boost::system::error_code &error, std::size_t bytes) {

    if (bytes > 0 && !error) {
        std::lock_guard<std::shared_mutex> guard(frame_guard);
        auto buf = last_frame;
        const size_t led_count = bytes / sizeof(HSV);
        if (buf == nullptr || led_count != buf->size())
            buf = std::make_shared<AllocatedBuffer<HSV>>(led_count);

        for (size_t i = 0; i < buf->size(); i++) {
            buf->at(i) = recv_buffer[i];
        }
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
    recv();
    worker_thread = std::thread(std::bind(&HSVUDPSink::run, this));
}

void HSVUDPSink::stop() {
    if (!io_service.stopped())
        io_service.stop();
    if (worker_thread.joinable())
        worker_thread.join();
}


std::shared_ptr<AbstractBaseBuffer<HSV>> HSVUDPSink::get() {
    std::shared_lock<std::shared_mutex> guard(frame_guard);

    return last_frame;
}

void HSVUDPSink::clear() {
    std::shared_lock<std::shared_mutex> guard(frame_guard);
    last_frame = nullptr;
}
