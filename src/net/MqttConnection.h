#pragma once

#include <mqtt_client_cpp.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include "../VariableStore/VariableStore.h"

class MqttConnection {
    bool thread_running;
    const std::string realm;
    std::shared_ptr<VariableStore> store;
    boost::asio::io_service io_service;
    std::shared_ptr<mqtt::client<boost::asio::ip::tcp::socket, mqtt::null_strand>> mqtt_client;
    std::thread worker_thread;


public:
    MqttConnection(std::shared_ptr<VariableStore>& store, const std::string& broker, const std::string& realm);

    void stop() {
        if (!io_service.stopped())
            io_service.stop();
    }

    ~MqttConnection() {
        stop();
        worker_thread.join();
    }

private:
    void run() {
        thread_running = true;
        try {
            std::cout << io_service.run() << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        thread_running = false;
    }

    bool connack_handler(bool sp, std::uint8_t connack_return_code);

    bool publish_handler(std::uint8_t fixed_header,
                         boost::optional<std::uint16_t> packet_id,
                         std::string topic_name,
                         std::string contents);

    void close_handler();
};
