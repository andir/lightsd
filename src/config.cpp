//
// Created by andi on 10/12/16.
//
#include <yaml-cpp/yaml.h>
#include <boost/algorithm/string.hpp>
#include "config.h"
#include "config_sequence.h"
#include "config_output.h"

ConfigPtr parseConfig(const std::string &filename) {
    auto config = std::make_unique<Config>();

    YAML::Node yaml_config = YAML::LoadFile(filename);


    if (yaml_config["width"]) {
        config->width = yaml_config["width"].as<size_t>();
    } else {
        config->width = 15;
    }

    if (yaml_config["fps"]) {
        config->fps = yaml_config["fps"].as<size_t>();
    } else {
        config->fps = 25;
    }

    if (!yaml_config["sequence"]) {
        throw ConfigParsingException("No sequence configured.");
    } else {
        const YAML::Node sequence_node = yaml_config["sequence"];
        auto s = config->store.get();
        parseSequence(*s, config->sequence, sequence_node);
    }

    if (!yaml_config["outputs"]) {
        throw ConfigParsingException("No outputs defined.");
    } else {
        const YAML::Node outputs_node = yaml_config["outputs"];
        parseOutputs(config->outputs, outputs_node);

    }

    if (yaml_config["mqtt"]) {
        const auto mqtt = yaml_config["mqtt"];

        if (mqtt["broker"]) {
            const std::string broker = mqtt["broker"].as<std::string>();

            const std::string realm = [&mqtt]() -> std::string {
                if (mqtt["realm"]) {
                    return mqtt["realm"].as<std::string>();
                } else {
                    return "/lights/";
                }
            }();
            config->mqtt = std::make_unique<MqttConnection>(config->store, broker, realm);
        }
    }

    return config;
}


Config::Config() :
        fps(30),
        width(100),
        store(std::make_shared<VariableStore>()) {
}

Config::~Config() {
    mqtt = nullptr;
    sequence.clear();
    outputs.clear();
}
