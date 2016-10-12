//
// Created by andi on 10/12/16.
//
#include <boost/algorithm/string.hpp>
#include "config.h"
#include "RainbowOperation.h"
#include "RotateOperation.h"
#include "WebsocketOutputWrapper.h"
#include "UDPOutputWrapper.h"


struct ConfigParsingException : public std::exception {
    const std::string s;

    ConfigParsingException(std::string s) : s(s) {}

    const char *what() const throw() {
        return s.c_str();
    }
};


template<typename IteratorType1, typename IteratorType2>
std::unique_ptr<Operation> generateSequenceStep(VariableStore& store, const std::string &step_type, IteratorType1 begin, IteratorType2 end) {
    using namespace std::string_literals;

    static const std::set<std::string> known_sequence_types = {
            "initrainbow"s, "rotate"s, "initsolidcolor"s, "shade"s, "fade"s
    };


    const auto lower_case_name = boost::algorithm::to_lower_copy(step_type);
    const auto it = known_sequence_types.find(lower_case_name);

    if (it == known_sequence_types.end()) {
        throw ConfigParsingException("The selected step_type wasn't found.");
    } else {
        if (lower_case_name == "initrainbow") {
            return std::make_unique<RainbowOperation>(store, begin, end);
        } else if (lower_case_name == "rotate") {
            return std::make_unique<RotateOperation>(store, begin, end);
        } else if (lower_case_name == "initsolidcolor") {
            return std::make_unique<SolidColorOperation>(store, begin, end);
        } else if (lower_case_name == "shade") {
            return std::make_unique<ShadeOperation>(store, begin, end);
        } else if (lower_case_name == "fade") {
            return std::make_unique<FadeOperation>(store, begin, end);
        } else {
            return nullptr;
        }
    }
};

void parseSequence(VariableStore& store, std::vector<std::unique_ptr<Operation>> &steps, const YAML::Node &sequence_node) {
    assert(sequence_node.Type() == YAML::NodeType::Sequence);

    for (const auto node : sequence_node) {
        switch (node.Type()) {
            case YAML::NodeType::Scalar: {
                const std::string step_name = node.as<std::string>();

                auto step = generateSequenceStep(store, step_name, node.end(),
                                                 node.end());
                steps.push_back(std::move(step));
                break;

            }
            case YAML::NodeType::Map: {

                for (const auto &map_entry  : node) {
                    const std::string step_name = map_entry.first.as<std::string>();
                    std::cout << step_name << std::endl;


                    switch (map_entry.second.Type()) {
                        case YAML::NodeType::Map: {
                            auto step = generateSequenceStep(store, step_name, map_entry.second.begin(),
                                                             map_entry.second.end());
                            steps.push_back(std::move(step));
                            break;
                        }
                        default:
                            std::cout << map_entry.second.Type() << std::endl;
                            throw ConfigParsingException("Invalid node in sequence.");
                    }

                }

                break;
            }
            case YAML::NodeType::Sequence:
            case YAML::NodeType::Undefined:
            case YAML::NodeType::Null:
                throw ConfigParsingException("Invalid node in sequence.");
        }
    }
}


void parseOutputs(std::map<std::string, std::shared_ptr<Output>> &outputs, const YAML::Node &outputs_node) {
    using namespace std::string_literals;
    assert(outputs_node.Type() == YAML::NodeType::Map);

    for (const auto node : outputs_node) {
        const std::string output_name = node.first.as<std::string>();

        if (outputs.find(output_name) != outputs.end()) {
            throw ConfigParsingException("Output already known.");
        }

        if (node.second.Type() != YAML::NodeType::Map) {
            throw ConfigParsingException("Outputs must be formatted as dict.");
        }


        static const std::set<std::string> known_types = {
                "websocket"s,
                "udp"s,
        };
        const std::string type = node.second["type"].as<std::string>();

        if (known_types.find(type) == known_types.end()) {
            throw ConfigParsingException("Unknown output type");
        }

        const auto params = node.second["params"];

        if (params.Type() != YAML::NodeType::Map) {
            throw ConfigParsingException("Params must be formatted as dict.");
        }

        if (type == "websocket") {
            auto p = std::make_unique<WebsocketOutputWrapper>(params);
            outputs[output_name] = std::move(p);
        } else if (type == "udp") {
            auto p = std::make_unique<UDPOutputWrapper>(params);
            outputs[output_name] = std::move(p);
        } else {
            throw ConfigParsingException("Unknown output type.");
        }

    }
}



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
        parseSequence(config->store, config->sequence, sequence_node);
    }

    if (!yaml_config["outputs"]) {
        throw ConfigParsingException("No outputs defined.");
    } else {
        const YAML::Node outputs_node = yaml_config["outputs"];
        parseOutputs(config->outputs, outputs_node);

    }

    return config;
}