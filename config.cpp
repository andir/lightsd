//
// Created by andi on 10/12/16.
//
#include <yaml-cpp/yaml.h>
#include <boost/algorithm/string.hpp>
#include "config.h"
#include "outputs/WebsocketOutputWrapper.h"
#include "outputs/UDPOutputWrapper.h"
#include "outputs/HSVUDPOutput.h"
#include "outputs/SharedMemoryOutput.h"
#include "outputs/DevMemOutput.h"

#include "operations/RainbowOperation.h"
#include "operations/RotateOperation.h"
#include "operations/RaindropOperation.h"
#include "operations/BellOperation.h"
#include "operations/SplashdropOperation.h"
#include "operations/HSVUDPInputOperation.h"
#include "operations/ShadeOperation.h"
#include "operations/SolidColorOperation.h"
#include "operations/FadeOperation.h"
#include "operations/lua/LuaOperation.h"
#include "operations/GameOfLifeOperation.h"

#include "outputs/spi/SPIOutput.h"

template<typename Iter1, typename Iter2> 
struct SequenceGenerator {
        virtual std::unique_ptr<Operation> operator()(VariableStore&, Iter1, Iter2) = 0;
};

template<typename T, typename Iter1, typename Iter2>
struct SSequenceGenerator : SequenceGenerator<Iter1, Iter2> {
        virtual std::unique_ptr<Operation> operator()(VariableStore &store, Iter1 begin, Iter2 end) {
                return std::make_unique<T>(store, begin, end);
        }
};
template<typename T, typename Iter1, typename Iter2, typename GeneratorType=SequenceGenerator<Iter1, Iter2> >
inline std::unique_ptr<GeneratorType> make_generator() {
        return std::make_unique<SSequenceGenerator<T, Iter1, Iter2>>();
}

template<typename Iter1, typename Iter2>
std::unique_ptr<Operation>
generateSequenceStep(VariableStore &store, const std::string &step_type, Iter1 begin, Iter2 end) {
    using GeneratorType = std::unique_ptr<SequenceGenerator<Iter1, Iter2> >;

    static std::map<std::string, GeneratorType> types;
    if (types.size() == 0) {
            types["bell"] = make_generator<BellOperation, Iter1, Iter2>();
            types["gameoflife"] = make_generator<GameOfLifeOperation, Iter1, Iter2>();
            types["hsvudpinput"] = make_generator<HSVUDPInputOperation, Iter1, Iter2>();
            types["initrainbow"] = make_generator<RainbowOperation, Iter1, Iter2>();
            types["initsolidcolor"] = make_generator<SolidColorOperation, Iter1, Iter2>();
            types["lua"] = make_generator<LuaOperation, Iter1, Iter2>();
            types["raindrop"] = make_generator<RaindropOperation, Iter1, Iter2>();
            types["rotate"] = make_generator<RotateOperation, Iter1, Iter2>();
            types["shade"] = make_generator<ShadeOperation, Iter1, Iter2>();
            types["splashdrop"] = make_generator<SplashdropOperation, Iter1, Iter2>();
            types["udpinput"] = make_generator<HSVUDPInputOperation, Iter1, Iter2>();
    }

    const auto lower_case_name = boost::algorithm::to_lower_copy(step_type);
    const auto it = types.find(lower_case_name);

    if (it == types.end()) {
        throw ConfigParsingException("The selected step_type wasn't found.");
    } else {
        auto& func = *(it->second);
        return func(store, begin, end);
    }
};

void
parseSequence(VariableStore &store, std::vector<std::unique_ptr<Operation>> &steps, const YAML::Node &sequence_node) {
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
                "shm"s,
                "hsvudp"s,
                "spi"s,
                "devmem"s,
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
        } else if (type == "shm") {
            auto p = std::make_unique<SharedMemoryOutput>(params);
            outputs[output_name] = std::move(p);
        } else if (type == "hsvudp") {
            auto p = std::make_unique<HSVUDPOutput>(params);
            outputs[output_name] = std::move(p);
        } else if (type == "spi") {
            auto p = std::make_unique<SPIOutput>(params);
            outputs[output_name] = std::move(p);
        } else if (type == "devmem") {
            auto p = std::make_unique<DevMemOutput>(params);
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
        store(std::make_shared<VariableStore>()) {
}

Config::~Config() {
    mqtt = nullptr;
    sequence.clear();
    outputs.clear();
}
