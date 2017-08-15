#include "GameOfLifeOperation.h"
#include "../config.h"
#include "../utils/random.h"


GameOfLifeOperation::GameOfLifeOperation(const std::string& name, VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
        Operation(name, store, start, end),
        initialized(false),
        frame_counter(0),
        default_hue(name + "/default_hue", Operation::HSV_HUE, store,
                    getValueByKey<float>("default_hue", start, end, 180.0f)),
        default_saturation(name + "/default_saturation", Operation::HSV_SATURATION, store,
                           getValueByKey<float>("default_saturation", start, end, 0.5f)),
        default_value(name + "/default_value", Operation::HSV_VALUE, store,
                      getValueByKey<float>("default_value", start, end, 1.0f)),
        v0(name + "/v0", Operation::BOOLEAN, store, getValueByKey<bool>("v0", start, end, false)),
        v1(name + "/v1", Operation::BOOLEAN, store, getValueByKey<bool>("v1", start, end, true)),
        v2(name + "/v2", Operation::BOOLEAN, store, getValueByKey<bool>("v2", start, end, true)),
        v3(name + "/v3", Operation::BOOLEAN, store, getValueByKey<bool>("v3", start, end, false)),
        v4(name + "/v4", Operation::BOOLEAN, store, getValueByKey<bool>("v4", start, end, true)),
        v5(name + "/v5", Operation::BOOLEAN, store, getValueByKey<bool>("v5", start, end, true)),
        v6(name + "/v6", Operation::BOOLEAN, store, getValueByKey<bool>("v6", start, end, true)),
        v7(name + "/v7", Operation::BOOLEAN, store, getValueByKey<bool>("v7", start, end, false)),
        speed(name + "/speed", Operation::BOOLEAN, store, getValueByKey<float>("speed", start, end, 1.0f)),
        randomizeColor(name + "/randomizeColor", Operation::BOOLEAN, store,
                       getValueByKey<bool>("randomizeColor", start, end, true)) {
}


void GameOfLifeOperation::update(const size_t width, const size_t fps) {
    if (!isEnabled()) {
        initialized = false;
        frame_counter = 0;
        return;
    }

    const auto led_count = width;
    if (this->delta.capacity() != led_count) {
        this->delta.clear();
        this->output.clear();
        this->delta.resize(led_count);
        this->output.resize(led_count);
        initialized = false;
        frame_counter = 0;
    }

    // Initialize the buffer with random state being populated
    if (!initialized) {
        initialized = true;
        const auto def_value = default_value;
        for (auto &led : this->output) {
            float value = 0.0f;
            if (random_int_in_range(0, 100) < 5) {
                value = def_value;
            }
            float hue;
            if (randomizeColor) {
                hue = random_int_in_range(0, 360);
            } else {
                hue = default_hue;
            }
            led = HSV{hue, default_saturation, value};
        }
    }

    // main update loop
    if (frame_counter % int(fps * speed) == 0) {
        std::vector<HSV> state(output.size());
        // once a second recalculate a new state
        auto default_color = HSV{default_hue, default_saturation, 0.0};
        for (size_t i = 0; i < output.size(); i++) {
            const size_t l_index = i == 0 ? output.size() - 1 : i - 1;
            const size_t r_index = i == output.size() - 1 ? 0 : i + 1;

            const auto &t = output.at(i);
            const auto &l = output[l_index];
            const auto &r = output[r_index];
            uint8_t parent_value = 0;
            if (l.value > 0.1) {
                parent_value += 4;
            }

            if (t.value > 0.1) {
                parent_value += 2;
            }

            if (r.value > 0.1) {
                parent_value += 1;
            }

            const auto new_value = [&]() {
                switch (parent_value) {
                    case 7:
                        return (float) v7;
                    case 6:
                        return (float) v6;
                    case 5:
                        return (float) v5;
                    case 4:
                        return (float) v4;
                    case 3:
                        return (float) v3;
                    case 2:
                        return (float) v2;
                    case 1:
                        return (float) v1;
                    case 0:
                        return (float) v0;
                    default:
                        return 0.0f;
                }
            }();

            auto &e = state[i];
            if (e.value <= 0.0f) {
                e = default_color;
                if (randomizeColor) {
                    e.hue = random_int_in_range(0, 360);
                }
            }
            e.value = new_value;
        }

        // recalc the delta after each new frame
        std::transform(state.begin(), state.end(), output.begin(), delta.begin(),
                       [fps](const HSV &a, const HSV &b) -> HSV {
                           return HSV{
                                   (a.hue - b.hue) / fps,
                                   (a.saturation - b.saturation) / fps,
                                   (a.value - b.value) / fps
                           };
                       });
    }

    // update output buffer
    std::transform(delta.begin(), delta.end(), output.begin(), output.begin(), [](const HSV &a, const HSV &b) -> HSV {
        return HSV{
                a.hue + b.hue,
                a.saturation + b.saturation,
                a.value + b.value
        };
    });

    frame_counter += 1;
    frame_counter %= fps;
}

Operation::BufferType GameOfLifeOperation::operator()(Operation::BufferType &buffer) {
    size_t i = 0;
    for (const auto &o : output) {
        auto &e = buffer->at(i);
        e = o;
        i++;
    }
    return buffer;
}
