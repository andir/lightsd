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
#include "operations/SequenceOperation.h"

// We can solve this entirely using the typesystem and a custom make_unique
// implementation but it is not worth the effort...
template<typename T>
inline std::unique_ptr<Operation> generator(const std::string& name, std::shared_ptr<VariableStore>& s, YAML::const_iterator begin, YAML::const_iterator end){
        return std::make_unique<T>(name, s, begin, end);
}

std::unique_ptr<Operation>
generateSequenceStep(const std::string& name, std::shared_ptr<VariableStore>& store, const std::string &step_type, YAML::const_iterator begin, YAML::const_iterator end) {
    using FuncT = std::unique_ptr<Operation> (*)(const std::string& name, std::shared_ptr<VariableStore>& store, YAML::const_iterator begin, YAML::const_iterator end);

    const static std::map<std::string, FuncT> types {
            {"fade", &generator<FadeOperation>},
            {"bell", &generator<BellOperation>},
            {"gameoflife", &generator<GameOfLifeOperation>},
            {"hsvudpinput", &generator<HSVUDPInputOperation>},
            {"rainbow", &generator<RainbowOperation>},
            {"initsolidcolor", &generator<SolidColorOperation>},
            {"lua", &generator<LuaOperation>},
            {"raindrop", &generator<RaindropOperation>},
            {"rotate", &generator<RotateOperation>},
            {"shade", &generator<ShadeOperation>},
            {"splashdrop", &generator<SplashdropOperation>},
            {"udpinput", &generator<HSVUDPInputOperation>},
            {"sequence", &generator<SequenceOperation>},
    };

    const auto lower_case_name = boost::algorithm::to_lower_copy(step_type);

    if (const auto it = types.find(lower_case_name); it == types.end()) {
        throw ConfigParsingException("The selected step_type wasn't found:" + lower_case_name);
    } else {
        auto& func = *(it->second);
        return func(name, store, begin, end);
    }
}

std::unique_ptr<Operation>
generateOperation(const std::string& name, std::shared_ptr<VariableStore>& store, const YAML::Node &config_node) {
    assert(config_node.Type() == YAML::NodeType::Map);
    const auto& operation_type = config_node["type"].as<std::string>();
    const auto& operation_params = config_node["params"];
    return generateSequenceStep(name, store, operation_type, operation_params.begin(), operation_params.end());
}
