#include <yaml-cpp/yaml.h>
#include "../VariableStore/ConcreteValueType.h"
#include "../VariableStore/VariableStore.h"
#include "../VariableStore/BoundConcreteValueType.h"
#include "../algorithm.h"
#include "../hsv.h"


class FadeOperation : public Operation {
    BoundConcreteValue<float> min;
    BoundConcreteValue<float> max;
    BoundConcreteValue<int> from;
    BoundConcreteValue<int> to;

    std::vector<float> mask;

public:
    FadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("fade", store, start, end),
            min("fade/min", Operation::FLOAT_0_1, store, getValueByKey<float>("min", start, end)),
            max("fade/max", Operation::FLOAT_0_1, store, getValueByKey<float>("max", start, end)),
            from("fade/from", Operation::INT, store, getValueByKey<int>("from", start, end)),
            to("fade/to", Operation::INT, store, getValueByKey<int>("to", start, end)) {

        // FIXME: as soon as the vars are observable recalculate the mask on change
        if (to.getValue() <= from.getValue() || min.getValue() >= max.getValue())
            return;

        const size_t length = (to.getValue() - from.getValue());
        const float diff = max.getValue() - min.getValue();
        const float step = diff / length;
        for (size_t i = 0; i < length; i++) {
            mask.push_back(step * i);
        }
    }

    BufferType operator()(BufferType &buffer) {
        algorithm::PartialMaskBuffer(mask, *buffer, from.getValue());
    }
};


