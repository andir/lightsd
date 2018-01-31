#pragma once

#include "Operation.h"

class FlashOperation : public Operation {
  enum State {
    IDLE,
    WAIT,
    FADE_IN,
    FADE_OUT,
  } state;

  inline static const char* toString(State s) {
    switch (s) {
      case State::IDLE:
        return "IDLE";
      case State::WAIT:
        return "WAIT";
      case State::FADE_IN:
        return "FADE_IN";
      case State::FADE_OUT:
        return "FADE_OUT";
      default:
        return "UNKNOWN";
    }
  }

  int64_t time_passed;

  MeasureTime<> time_measurement;

  BoundConcreteValue<float> hue;
  BoundConcreteValue<int> fade_milliseconds;
  BoundConcreteValue<int> wait_milliseconds;
  BoundConcreteValue<int> iteration_count;

  int iteration;

 public:
  FlashOperation(const std::string& name,
                 std::shared_ptr<VariableStore> store,
                 YAML::const_iterator begin,
                 YAML::const_iterator end);

  virtual ~FlashOperation() {}

  virtual Operation::BufferType operator()(BufferType& buffer);

  virtual void update(const size_t width, const size_t fps);
};
