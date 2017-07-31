#include "config.h"
#include "config_output.h"
#include "outputs/WebsocketOutputWrapper.h"
#include "outputs/UDPOutputWrapper.h"
#include "outputs/HSVUDPOutput.h"
#include "outputs/SharedMemoryOutput.h"
#include "outputs/DevMemOutput.h"
#include "outputs/spi/SPIOutput.h"

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


