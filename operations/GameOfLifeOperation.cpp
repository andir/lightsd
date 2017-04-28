#include "GameOfLifeOperation.h"
#include "../config.h"
#include "../utils/random.h"


GameOfLifeOperation::GameOfLifeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
        Operation("gameoflife", store, start, end),
        initialized(false),
        frame_counter(0),
        default_hue("gameoflife/default_hue", Operation::HSV_HUE, store, getValueByKey<float>("default_hue", start, end, 180.0f)),
        default_saturation("gameoflife/default_saturation", Operation::HSV_SATURATION, store, getValueByKey<float>("default_saturation", start, end, 0.5f)),
        default_value("gameoflife/default_value", Operation::HSV_VALUE, store, getValueByKey<float>("default_value", start, end, 1.0f)),
        v0("gameoflife/v0", Operation::BOOLEAN, store, getValueByKey<bool>("v0", start, end, false)),
        v1("gameoflife/v1", Operation::BOOLEAN, store, getValueByKey<bool>("v1", start, end, true)),
        v2("gameoflife/v2", Operation::BOOLEAN, store, getValueByKey<bool>("v2", start, end, true)),
        v3("gameoflife/v3", Operation::BOOLEAN, store, getValueByKey<bool>("v3", start, end, false)),
        v4("gameoflife/v4", Operation::BOOLEAN, store, getValueByKey<bool>("v4", start, end, true)),
        v5("gameoflife/v5", Operation::BOOLEAN, store, getValueByKey<bool>("v5", start, end, true)),
        v6("gameoflife/v6", Operation::BOOLEAN, store, getValueByKey<bool>("v6", start, end, true)),
        v7("gameoflife/v7", Operation::BOOLEAN, store, getValueByKey<bool>("v7", start, end, false))
{
}


void GameOfLifeOperation::update(const Config* const cfg) {
    if (!isEnabled()) {
        initialized = false;
        frame_counter = 0;
        return;
    }

    const auto led_count = cfg->width;
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
        const auto def_value = default_value.getValue();
        for (auto& led : this->output) {
            float value = 0.0f;
            if (random_int_in_range(0, 100) < 5) {
                value = def_value;
            }
            led = HSV{default_hue.getValue(), default_saturation.getValue(), value};
        }
    }

    // main update loop
    if (frame_counter % cfg->fps == 0) {
        std::vector<HSV> state(output.size());
        // once a second recalculate a new state
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
                        return (float) v7.getValue();
                    case 6:
                        return (float) v6.getValue();
                    case 5:
                        return (float) v5.getValue();
                    case 4:
                        return (float) v4.getValue();
                    case 3:
                        return (float) v3.getValue();
                    case 2:
                        return (float) v2.getValue();
                    case 1:
                        return (float) v1.getValue();
                    case 0:
                        return (float) v0.getValue();
                    default:
                        return 0.0f;
                }
            }();
            state[i] = t;
            state[i].value = new_value;
        }

        // recalc the delta after each new frame
        std::transform(state.begin(), state.end(), output.begin(), delta.begin(), [cfg](const HSV& a, const HSV& b) -> HSV {
            return HSV{
                    (a.hue - b.hue)/cfg->fps,
                    (a.saturation - b.saturation)/cfg->fps,
                    (a.value - b.value)/cfg->fps
            };
        });
    }

    // update output buffer
    std::transform(delta.begin(), delta.end(), output.begin(), output.begin(), [](const HSV& a, const HSV& b) -> HSV {
        return HSV{
                a.hue + b.hue,
                a.saturation + b.saturation,
                a.value + b.value
        };
    });

    frame_counter += 1;

    frame_counter %= cfg->fps;
}

Operation::BufferType GameOfLifeOperation::operator()(Operation::BufferType &buffer) {
    size_t i = 0;
    for (const auto& o : output) {
        auto& e = buffer->at(i);
        e = o;
        i++;
    }
    return buffer;
}