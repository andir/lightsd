#include "DoorBuzzerOperation.h"

#include <algorithm.h>
#include <hsv.h>

DoorBuzzerOperation::DoorBuzzerOperation(const std::string& name,
                                         std::shared_ptr<VariableStore> store,
                                         YAML::const_iterator begin,
                                         YAML::const_iterator end)
    : Operation(name, store, begin, end),
      state(State::IDLE),
      hue(name + "/hue",
          Operation::HSV_HUE,
          store,
          getValueByKey<float>("hue", begin, end, 120.0f)),
      fade_milliseconds(name + "/fade_duration",
                        Operation::INT,
                        store,
                        getValueByKey<int>("fade_duration", begin, end, 1050)),
      wait_milliseconds(name + "/wait_duration",
                        Operation::INT,
                        store,
                        getValueByKey<int>("wait_duration", begin, end, 700)),
      doorbell_name(
          getValueByKey<std::string>("doorbell_name",
                                     begin,
                                     end,
                                     "sub-sequence-door:bell/enabled")) {
  doorbell_ptr = store->getVar(doorbell_name);
}

Operation::BufferType DoorBuzzerOperation::operator()(
    Operation::BufferType& buffer) {
  HSV color;

  switch (state) {
    case State::FADE_IN: {
      assert(fade_milliseconds >
             0);  // FIXME: check during startup / config parsing
      const float step = float(time_passed) / fade_milliseconds;
      color = HSV{hue.getValue(), 1.0f, std::min(0.2f + step, 1.0f)};
      break;
    }
    case State::WAIT:
      color = HSV{hue.getValue(), 1.0f, 1.0f};
      break;
    default:
      return buffer;
  }

  for (auto& led : *buffer) {
    led = color;
  }

  return buffer;
}

void DoorBuzzerOperation::update(const size_t width, const size_t fps) {
  if (!isEnabled()) {
    state = State::IDLE;
    return;
  }

  time_passed = time_measurement.measure();

  switch (state) {
    case State::IDLE:
      time_measurement.reset();
      time_passed = 0;
      iteration = 0;
      // fallthrough
    case State::FADE_IN:
      if (time_passed < fade_milliseconds) {
        // stay
        state = State::FADE_IN;
      } else {
        // transition
        state = State::WAIT;
      }
      break;
    case State::WAIT:
      if (time_passed < fade_milliseconds + wait_milliseconds) {
        // stay
        state = State::WAIT;
      } else {
        // disable doorbell instance
        if (auto doorbell = doorbell_ptr.lock()) {
          doorbell->setBool(false);
        }
        // stop animation
        enabled = false;
        state = State::IDLE;
      }
      break;
  }
};
