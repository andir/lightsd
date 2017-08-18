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



    if (!yaml_config["operations"]) {
        throw ConfigParsingException("No operations configured.");
    } else {
        const YAML::Node operations_node = yaml_config["operations"];
        for (const auto& operation_node : operations_node) {
                const std::string operation_name = operation_node.first.as<std::string>();
                if (config->operations[operation_name]) {
                        throw ConfigParsingException("Duplicate operation.");
                } else {
                        const auto& n = operation_node.second;
                        config->operations[operation_name] = generateOperation(operation_name, config->store, n);
                }
        }
    }

    if (!yaml_config["sequence"]) {
        throw ConfigParsingException("No sequence configured.");
    } else {
        const YAML::Node sequences_node = yaml_config["sequence"];
        if (sequences_node.Type() != YAML::NodeType::Sequence) {
                throw ConfigParsingException("sequences must be a sequence of strings");
        }

        for (const auto& sequence_node : sequences_node) {
                const auto& sequence_step_name = sequence_node.as<std::string>();
                auto& operation_it = config->operations[sequence_step_name];
                if (!operation_it) {
                        std::cerr << sequence_step_name << std::endl;
                        throw ConfigParsingException("Missing sequence step.");
                } else {
                        config->sequence.push_back(operation_it);
                }
        }
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
    mqtt.reset();
}
