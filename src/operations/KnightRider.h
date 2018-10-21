#pragma once

#include "Operation.h"

class KnightRiderOperation : public Operation {
  enum State {
    IDLE,
    RUNNING,
  } state;

  enum Direction {
    ASCENDING,
    DESCENDING,
  } direction;

  int currentCenter = 0;

  BoundConcreteValue<float> hue;
  BoundConcreteValue<float> saturation;
  BoundConcreteValue<float> value;

  BoundConcreteValue<int> duration_milliseconds;
  BoundConcreteValue<float> width;

  float getShade() const;

 public:
  KnightRiderOperation(const std::string& name,
                       std::shared_ptr<VariableStore> store,
                       YAML::const_iterator begin,
                       YAML::const_iterator end);

  virtual ~KnightRiderOperation() {}

  virtual Operation::BufferType operator()(Operation::BufferType& buffer);

  virtual void update(const size_t width, const size_t fps);
};
