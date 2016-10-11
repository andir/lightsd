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
    std::string realm;
    VariableStore &store;
    boost::asio::io_service io_service;
    std::shared_ptr<mqtt::client<boost::asio::ip::tcp::socket, mqtt::null_strand>> mqtt_client;
    std::thread worker_thread;


public:
    MqttVarStore(VariableStore &store, const std::string broker, const std::string realm) :
            realm(realm),
            store(store),
            mqtt_client(mqtt::make_client_no_strand(io_service, broker, 1883)) {


        mqtt_client->set_client_id("foo");
        mqtt_client->set_clean_session(true);

        mqtt_client->set_publish_handler(
                [this]
                        (std::uint8_t header,
                         boost::optional<std::uint16_t> packet_id,
                         std::string topic_name,
                         std::string contents) {
                    std::cout << "publish received. "
                              << "dup: " << std::boolalpha << mqtt::publish::is_dup(header)
                              << " pos: " << mqtt::qos::to_str(mqtt::publish::get_qos(header))
                              << " retain: " << mqtt::publish::is_retain(header) << std::endl;
                    if (packet_id)
                        std::cout << "packet_id: " << *packet_id << std::endl;
                    std::cout << "topic_name: " << topic_name << std::endl;
                    std::cout << "contents: " << contents << std::endl;
                    return true;
                });
        mqtt_client->set_error_handler(
                []
                        (boost::system::error_code const &ec) {
                    std::cout << "error: " << ec.message() << std::endl;
                });
        mqtt_client->set_connack_handler([this, &store]
                                                 (bool sp, std::uint8_t connack_return_code) {
            std::cerr << "connack return code: " << connack_return_code << std::endl;
            for (const auto &e : store.keys()) {
                std::stringstream key_ss, value_ss;
                key_ss << this->realm << e;
                const auto &val = store.getVar(e);
                value_ss << *val;
                mqtt_client->publish(key_ss.str(), value_ss.str());
                key_ss << "/set";
                mqtt_client->subscribe(key_ss.str(), mqtt::qos::at_least_once);
            }

            return true;

        });

        mqtt_client->set_publish_handler([this](std::uint8_t fixed_header,
                                                boost::optional<std::uint16_t> packet_id,
                                                std::string topic_name,
                                                std::string contents) {

            std::cout << "topic: " << topic_name << std::endl;
            std::cout << "value: " << contents << std::endl;


            if (boost::algorithm::starts_with(topic_name, this->realm)) {
                const auto e = topic_name.find(this->realm);
                if (e != topic_name.size()) {
                    std::string s(topic_name, e + this->realm.size(), topic_name.size() - e);
                    const auto& keys = this->store.keys();
                    const auto it = keys.find(s);
                    if (it != keys.end()) {
                        auto spt = this->store.getVar(s);
                        if (spt != nullptr) {
                            try {

                                switch (spt->getType()) {
                                    case ValueType::Type::FLOAT:
                                        spt->setFloat(boost::lexical_cast<float>(contents));
                                        break;
                                    case ValueType::Type::INTEGER:
                                        spt->setFloat(boost::lexical_cast<int>(contents));
                                        break;
                                    default:
                                        std::cerr << "Unbale to parse " << contents << std::endl;
                                }
                            } catch(boost::bad_lexical_cast& ex) {
                                std::cerr << ex.what() << std::endl;
                            }
                        }
                    }
                }
            }
            return true;
        });

        mqtt_client->set_close_handler(
                []
                        () {
                    std::cout << "closed." << std::endl;
                });


        mqtt_client->connect();
        worker_thread = std::thread(std::bind(&MqttVarStore::run, this));

    }

    ~MqttVarStore() {
        if (!io_service.stopped())
            io_service.stop();
        worker_thread.join();
    }

private:
    void run() {
        try {
            std::cout << io_service.run() << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

};


#endif //LIGHTSD_MQTTVARSTORE_H
