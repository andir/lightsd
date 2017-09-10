#include "config.h"
#include "config_output.h"
#include "outputs/WebsocketOutputWrapper.h"
#include "outputs/UDPOutputWrapper.h"
#include "outputs/HSVUDPOutput.h"
#include "outputs/SharedMemoryOutput.h"
#include "outputs/DevMemOutput.h"
#include "outputs/spi/SPIOutput.h"

// We can solve this entirely using the typesystem and a custom make_unique
// implementation but it is not worth the effort...
template<typename T>
inline std::unique_ptr<Output> generate(const YAML::Node &params, std::shared_ptr<VariableStore> &store) {
    if constexpr (std::is_constructible<T, const YAML::Node &, std::shared_ptr<VariableStore> &>::value) {
            return std::make_unique<T>(params, store);
    } else if constexpr (std::is_constructible<T, const YAML::Node &>::value) {
            return std::make_unique<T>(params);
    } else {
        static_assert("No valid constructor found.");
    }
}


void parseOutputs(std::map<std::string, std::shared_ptr<Output>> &outputs, const YAML::Node &outputs_node,
                  std::shared_ptr<VariableStore> &store) {
    assert(outputs_node.Type() == YAML::NodeType::Map);

    for (const auto node : outputs_node) {
        const std::string output_name = node.first.as<std::string>();

        if (outputs.find(output_name) != outputs.end()) {
            throw ConfigParsingException("Output already known.");
        }

        if (node.second.Type() != YAML::NodeType::Map) {
            throw ConfigParsingException("Outputs must be formatted as dict.");
        }

        const auto params = node.second["params"];

        if (params.Type() != YAML::NodeType::Map) {
            throw ConfigParsingException("Params must be formatted as dict.");
        }

        using FuncT = std::unique_ptr<Output> (*)(const YAML::Node &params, std::shared_ptr<VariableStore> &);
        static const std::map<std::string, FuncT> types{
                {"websocket", &generate<WebsocketOutputWrapper>},
                {"udp",       &generate<UDPOutputWrapper>},
                {"shm",       &generate<SharedMemoryOutput>},
                {"hsvudp",    &generate<HSVUDPOutput>},
                {"spi",       &generate<SPIOutput>},
                {"devmem",    &generate<DevMemOutput>}
        };

        const std::string type = node.second["type"].as<std::string>();

        if (const auto it = types.find(type);
        it == types.end()) {
            throw ConfigParsingException("Unknown output type ");
        } else {
            auto &func = *(it->second);
            outputs[output_name] = func(params, store);
        }
    }
}


