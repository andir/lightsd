#include <memory>
#include "config_sequence.h"
#include "config.h"
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

template<typename T>
inline std::unique_ptr<Operation> foo(VariableStore& s, YAML::const_iterator begin, YAML::const_iterator end){
        return std::make_unique<T>(s, begin, end);
}

std::unique_ptr<Operation>
generateSequenceStep(VariableStore &store, const std::string &step_type, YAML::const_iterator begin, YAML::const_iterator end) {

    using FuncT = std::unique_ptr<Operation> (VariableStore&, YAML::const_iterator begin, YAML::const_iterator end);

    const static std::map<std::string, FuncT*> types {
            {"fade", &foo<FadeOperation>},
            {"bell", &foo<BellOperation>},
            {"gameoflife", &foo<GameOfLifeOperation>},
            {"hsvudpinput", &foo<HSVUDPInputOperation>},
            {"initrainbow", &foo<RainbowOperation>},
            {"initsolidcolor", &foo<SolidColorOperation>},
            {"lua", &foo<LuaOperation>},
            {"raindrop", &foo<RaindropOperation>},
            {"rotate", &foo<RotateOperation>},
            {"shade", &foo<ShadeOperation>},
            {"splashdrop", &foo<SplashdropOperation>},
            {"udpinput", &foo<HSVUDPInputOperation>},
    };

    const auto lower_case_name = boost::algorithm::to_lower_copy(step_type);
    const auto it = types.find(lower_case_name);

    if (it == types.end()) {
        throw ConfigParsingException("The selected step_type wasn't found.");
    } else {
        auto& func = *(it->second);
        return func(store, begin, end);
    }
}

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


