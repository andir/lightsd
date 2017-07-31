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


struct SequenceGenerator {
        using IterT=YAML::const_iterator;
        virtual std::unique_ptr<Operation> operator()(VariableStore&, IterT, IterT) = 0;
};

template<typename T>
struct SSequenceGenerator : SequenceGenerator {
        using ParentT = SequenceGenerator;
        using IterT= typename ParentT::IterT;
        using Self = SSequenceGenerator<T>;
        inline static std::unique_ptr<ParentT> build() {
                return std::make_unique<Self>();
        }
        virtual std::unique_ptr<Operation> operator()(VariableStore &store, IterT begin, IterT end) {
                return std::make_unique<T>(store, begin, end);
        }
};

std::unique_ptr<Operation>
generateSequenceStep(VariableStore &store, const std::string &step_type, YAML::const_iterator begin, YAML::const_iterator end) {
    using GeneratorType = std::unique_ptr<SequenceGenerator>;

    static std::map<std::string, GeneratorType> types;
    if (types.size() == 0) {
            types["bell"] = SSequenceGenerator<BellOperation>::build();
            types["gameoflife"] = SSequenceGenerator<GameOfLifeOperation>::build();
            types["hsvudpinput"] = SSequenceGenerator<HSVUDPInputOperation>::build();
            types["initrainbow"] = SSequenceGenerator<RainbowOperation>::build();
            types["initsolidcolor"] = SSequenceGenerator<SolidColorOperation>::build();
            types["lua"] = SSequenceGenerator<LuaOperation>::build();
            types["raindrop"] = SSequenceGenerator<RaindropOperation>::build();
            types["rotate"] = SSequenceGenerator<RotateOperation>::build();
            types["shade"] = SSequenceGenerator<ShadeOperation>::build();
            types["splashdrop"] = SSequenceGenerator<SplashdropOperation>::build();
            types["udpinput"] = SSequenceGenerator<HSVUDPInputOperation>::build();
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

