//
// Created by andi on 10/11/16.
//

#ifndef LIGHTSD_MQTTVARSTORE_H
#define LIGHTSD_MQTTVARSTORE_H

#include <mqtt_client_cpp.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <experimental/string_view>
#include <thread>
#include "VariableStore/ValueType.h"
#include "VariableStore/VariableStore.h"

class MqttVarStore {
    bool thread_running;
    std::string realm;
    std::shared_ptr <VariableStore> store;
    boost::asio::io_service io_service;
    std::shared_ptr <mqtt::client<boost::asio::ip::tcp::socket, mqtt::null_strand>> mqtt_client;
    std::thread worker_thread;


public:
    MqttVarStore(std::shared_ptr <VariableStore> store, const std::string broker, const std::string realm);

    void stop() {
        if (!io_service.stopped())
            io_service.stop();
    }

    ~MqttVarStore() {
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

};


#endif //LIGHTSD_MQTTVARSTORE_H
