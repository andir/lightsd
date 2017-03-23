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
        std::lock_guard<std::mutex> l(mutex);

        if (to.getValue() <= from.getValue() || min.getValue() >= max.getValue())
            return;

        const size_t length = (to.getValue() - from.getValue());

        mask.resize(length);

        const float diff = max.getValue() - min.getValue();
        const float step = diff / length;
        for (size_t i = 0; i < length; i++) {
            mask.push_back(step * i);
        }
    }

    template<typename T>
    inline auto getCallback() {
        return [this](const T&) {
            this->recalcMask();
        };
    }

public:
    FadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("fade", store, start, end),
            min("fade/min", Operation::FLOAT_0_1, store, getValueByKey<float>("min", start, end), getCallback<float>()),
            max("fade/max", Operation::FLOAT_0_1, store, getValueByKey<float>("max", start, end), getCallback<float>()),
            from("fade/from", Operation::INT, store, getValueByKey<int>("from", start, end), getCallback<int>()),
            to("fade/to", Operation::INT, store, getValueByKey<int>("to", start, end), getCallback<int>()) {

        recalcMask();
    }



    BufferType operator()(BufferType &buffer) {
        std::lock_guard<std::mutex> l(mutex);
        algorithm::PartialMaskBuffer(mask, *buffer, from.getValue());
        return buffer;
    }
};


