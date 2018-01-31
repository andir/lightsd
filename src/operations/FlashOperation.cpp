#include "FlashOperation.h"

#include <hsv.h>

FlashOperation::FlashOperation(const std::string& name,
                               std::shared_ptr<VariableStore> store,
                               YAML::const_iterator begin,
                               YAML::const_iterator end)
    : Operation(name, store, begin, end),
      state(State::IDLE),
      hue(name + "/hue",
          Operation::HSV_HUE,
          store,
          getValueByKey<float>("hue", begin, end, 0.0f)),
      fade_milliseconds(name + "/fade_duration",
                        Operation::INT,
                        store,
                        getValueByKey<int>("fade_duration", begin, end, 850)),
      wait_milliseconds(name + "/wait_duration",
                        Operation::INT,
                        store,
                        getValueByKey<int>("wait_duration", begin, end, 550)),
      iteration_count(name + "/iteration_count",
                      Operation::INT,
                      store,
                      getValueByKey<int>("iteration_count", begin, end, 3)) {}

Operation::BufferType FlashOperation::operator()(
    Operation::BufferType& buffer) {
  HSV color;

  float value = 1.0f;
  float step = 0;
  const auto animation_progress =
      (time_passed - wait_milliseconds) % (+2 * fade_milliseconds);

  switch (state) {
    case State::WAIT:
      color = HSV{hue.getValue(), 1.0f, 1.0f};
      break;
    case State::FADE_IN:
      step =
          (animation_progress - fade_milliseconds) / float(fade_milliseconds);
      value = std::min(0.2f + step, 1.0f);
      color = HSV{hue.getValue(), 1.0f, value};
      break;
    case State::FADE_OUT:
      step =
          (fade_milliseconds - animation_progress) / float(fade_milliseconds);
      value = std::min(0.2f + step, 1.0f);
      color = HSV{hue.getValue(), 1.0f, value};
      break;
    default:
      return buffer;
  }

  for (auto& led : *buffer) {
    led = color;
  }

  return buffer;
}

void FlashOperation::update(const size_t width, const size_t fps) {
  if (!isEnabled()) {
    state = State::IDLE;
    return;
  }

  time_passed = time_measurement.measure();

  const auto animation_iteration =
      (time_passed - wait_milliseconds) / (2 * fade_milliseconds);
  const auto animation_progress =
      (time_passed - wait_milliseconds) % (2 * fade_milliseconds);

  switch (state) {
    case State::IDLE:
      time_measurement.reset();
      time_passed = 0;
      iteration = 0;
      // fallthrough
    case State::WAIT:
      if (time_passed < wait_milliseconds) {
        // stay
        state = State::WAIT;
      } else {
        // transition
        state = State::FADE_OUT;
      }
      break;
    case State::FADE_OUT:
      if (animation_progress < fade_milliseconds) {
        // stay
        state = State::FADE_OUT;
      } else {
        if (animation_iteration >= iteration_count) {
          // disable
          enabled = false;
          state = State::IDLE;
        } else {
          // transition
          state = State::FADE_IN;
        }
      }
      break;
    case State::FADE_IN:
      if (animation_progress >= fade_milliseconds) {
        // stay
        state = State::FADE_IN;
      } else {
        // transition
        state = State::FADE_OUT;
      }
      break;
  }

  iteration = animation_iteration;
};
