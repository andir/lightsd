//
// Created by andi on 10/11/16.
//

#include "MqttVarStore.h"

MqttVarStore::MqttVarStore(std::shared_ptr <VariableStore> store, const std::string broker, const std::string realm) :
        realm(realm),
        store(store),
        mqtt_client(mqtt::make_client_no_strand(io_service, broker, 1883)) {


    mqtt_client->set_client_id("foo");
    mqtt_client->set_clean_session(true);

    mqtt_client->set_publish_handler([this]
                                             (std::uint8_t header,
                                              boost::optional <std::uint16_t> packet_id,
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
    mqtt_client->set_error_handler([](boost::system::error_code const &ec) {
        std::cout << "error: " << ec.message() << std::endl;
    });
    mqtt_client->set_connack_handler([this]
                                             (bool sp, std::uint8_t connack_return_code) {
        std::cerr << "connack return code: " << int(connack_return_code) << std::endl;
        for (const auto &e : this->store->keys()) {
            std::stringstream key_ss, value_ss;
            key_ss << this->realm << e;
            const auto &val = this->store->getVar(e);
            value_ss << *val;
            mqtt_client->publish(key_ss.str(), value_ss.str());
            key_ss << "/set";
            mqtt_client->subscribe(key_ss.str(), mqtt::qos::at_least_once);
            std::cerr << "subscribed to " << key_ss.str() << std::endl;
        }

        return true;

    });

    mqtt_client->set_publish_handler([this](std::uint8_t fixed_header,
                                            boost::optional <std::uint16_t> packet_id,
                                            std::string topic_name,
                                            std::string contents) {

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
                                    std::cerr << "Unbale to parse " << contents << std::endl;
                            }
                        } catch (boost::bad_lexical_cast &ex) {
                            std::cerr << ex.what() << std::endl;
                        }
                        std::stringstream publish_ss, value_ss;

                        publish_ss << this->realm << s;
                        value_ss << *spt;

                        mqtt_client->publish(publish_ss.str(), value_ss.str());

                    }
                }
            }
        }
        return true;
    });

    mqtt_client->set_close_handler([]() {
        std::cout << "closed." << std::endl;
    });


    mqtt_client->connect();
    worker_thread = std::thread(std::bind(&MqttVarStore::run, this));

}