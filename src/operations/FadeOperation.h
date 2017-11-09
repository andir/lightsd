#pragma once

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
    std::mutex mutex;

    void recalcMask() {
        std::scoped_lock l(mutex);

        if (to <= from || min >= max)
            return;

        const size_t length = (to - from);

        mask.resize(length);

        const float diff = max - min;
        const float step = diff / length;
        for (size_t i = 0; i < length; i++) {
            mask[i] = min + step * i;
        }
    }

    inline auto getCallback() {
        return [this](ValueType*) {
            this->recalcMask();
        };
    }

public:
    FadeOperation(const std::string& name, std::shared_ptr<VariableStore> store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation(name, store, start, end),
            min(name + "/min", Operation::FLOAT_0_1, store, getValueByKey<float>("min", start, end), getCallback()),
            max(name + "/max", Operation::FLOAT_0_1, store, getValueByKey<float>("max", start, end), getCallback()),
            from(name + "/from", Operation::INT, store, getValueByKey<int>("from", start, end), getCallback()),
            to(name + "/to", Operation::INT, store, getValueByKey<int>("to", start, end), getCallback()) {

        recalcMask();
    }


    BufferType operator()(BufferType &buffer) {
        std::scoped_lock l(mutex);
        if (mask.size() == 0)
            return buffer;

        algorithm::PartialMaskBuffer(mask, *buffer, from.getValue());
        return buffer;
    }
};
