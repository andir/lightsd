#pragma once

#include "Operation.h"

class DoorBuzzerOperation : public Operation {
  enum State {
    IDLE,
    FADE_IN,
    WAIT,
  } state;

  inline static const char* toString(State s) {
    switch (s) {
      case State::IDLE:
        return "IDLE";
      case State::FADE_IN:
        return "FADE_IN";
      case State::WAIT:
        return "WAIT";
      default:
        return "UNKNOWN";
    }
  }

  int64_t time_passed;

  MeasureTime<> time_measurement;

  BoundConcreteValue<float> hue;

  BoundConcreteValue<int> fade_milliseconds;
  BoundConcreteValue<int> wait_milliseconds;

  std::string doorbell_name;
  std::weak_ptr<ValueType> doorbell_ptr;

  int iteration;

 public:
  DoorBuzzerOperation(const std::string& name,
                      std::shared_ptr<VariableStore> store,
                      YAML::const_iterator begin,
                      YAML::const_iterator end);

  virtual ~DoorBuzzerOperation() {}

  virtual Operation::BufferType operator()(BufferType& buffer);

  virtual void update(const size_t width, const size_t fps);
};
