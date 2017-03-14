//
// Created by andi on 10/11/16.
//

#include "MqttConnection.h"

#include <iostream>
#include <fstream>
#include <string>

static std::string getClientId() {
    static std::string mac;

    if (mac.length() != 0)
        return mac;

    std::ifstream f("/sys/class/net/eth0/address");

    if (f.good()) {
        std::string line;
        std::getline(f, line);
        return line;
    } else {
        return "foo";
    }
}


MqttConnection::MqttConnection(std::shared_ptr <VariableStore> store, const std::string broker, const std::string realm) :
        realm(realm),
        store(store),
        mqtt_client(mqtt::make_client_no_strand(io_service, broker, 1883)) {


    mqtt_client->set_client_id(getClientId());
    mqtt_client->set_clean_session(true);

    mqtt_client->set_error_handler([](boost::system::error_code const &ec) {
        std::cout << "MQTT error: " << ec.message() << std::endl;
    });

    mqtt_client->set_connack_handler([this](bool sp, std::uint8_t connack_return_code){
        return this->connack_handler(sp, connack_return_code);
    });

    mqtt_client->set_publish_handler([this](std::uint8_t fixed_header,
                                            boost::optional <std::uint16_t> packet_id,
                                            std::string topic_name,
                                            std::string contents) {
        return this->publish_handler(fixed_header, packet_id, topic_name, contents);
    });

    mqtt_client->set_close_handler([this]() {
        this->close_handler();
    });


    mqtt_client->connect();
    worker_thread = std::thread(std::bind(&MqttConnection::run, this));

}

bool MqttConnection::connack_handler(bool sp, std::uint8_t connack_return_code) {
    std::cerr << "connack return code: " << int(connack_return_code) << std::endl;
    for (const auto &e : this->store->keys()) {
        std::stringstream key_ss, value_ss;
        key_ss << this->realm << e;
        const auto &val = this->store->getVar(e);
        value_ss << *val;
        mqtt_client->publish(key_ss.str(), value_ss.str());
        mqtt_client->publish(key_ss.str() + "/type", this->store->getTypeName(e));
        key_ss << "/set";
        mqtt_client->subscribe(key_ss.str(), mqtt::qos::at_least_once);
        std::cerr << "subscribed to " << key_ss.str() << std::endl;
    }

    return true;
}

bool MqttConnection::publish_handler(std::uint8_t fixed_header, boost::optional<std::uint16_t> packet_id,
                                     std::string topic_name, std::string contents) {

    std::cout << "topic: " << topic_name << std::endl;
    std::cout << "value: " << contents << std::endl;

    static const std::string set("set");

    if (boost::algorithm::starts_with(topic_name, this->realm) &&
        boost::algorithm::ends_with(topic_name, set)) {

        const auto e = topic_name.find(this->realm);
        if (e != topic_name.size()) {
            const size_t len = topic_name.size() - this->realm.size() - e - set.size() - 1;
            std::string s(topic_name, this->realm.size(), len);

            const auto &keys = this->store->keys();
            const auto it = keys.find(s);

            if (it != keys.end()) {
                auto spt = this->store->getVar(s);
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
                                std::cerr << "Unable to parse " << contents << std::endl;
                        }
                    } catch (boost::bad_lexical_cast &ex) {
                        std::cerr << ex.what() << std::endl;
                    }
                    std::stringstream publish_ss, value_ss;

                    publish_ss << this->realm << s;
                    value_ss << *spt;

                    mqtt_client->publish(publish_ss.str(), value_ss.str(), mqtt::qos::at_most_once, true);
                }
            }
        }
    }
    return true;
}

void MqttConnection::close_handler() {
    std::cout << "mqtt connection closed." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "attempting reconnect to mqtt browker" << std::endl;
    this->mqtt_client->connect();

    if (!this->thread_running) {
        std::cout << "mqtt worker thread dead. Spawning new thread." << std::endl;
        this->worker_thread = std::thread(std::bind(&MqttConnection::run, this));
    }
}

