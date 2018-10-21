#include "KnightRider.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#include <iomanip>
#include <iostream>

KnightRiderOperation::KnightRiderOperation(const std::string& name,
                                           std::shared_ptr<VariableStore> store,
                                           YAML::const_iterator begin,
                                           YAML::const_iterator end)
    : Operation(name, store, begin, end),
      state(State::IDLE),
      value(name + "/value",
            Operation::HSV_VALUE,
            store,
            getValueByKey<float>("value", begin, end, 1.0f)),
      duration_milliseconds(name + "/duration",
                            Operation::INT,
                            store,
                            getValueByKey<int>("duration", begin, end, 1000)),
      width(name + "/width",
            Operation::FLOAT,
            store,
            getValueByKey<float>("width", begin, end, 0.1f)) {}

Operation::BufferType KnightRiderOperation::operator()(
    Operation::BufferType& buffer) {
  if (!isEnabled())
    return buffer;

  int size = (*buffer).size();

  for (int i = 0; i < size; i++) {
    if (direction == Direction::ASCENDING && i > currentCenter) {
      (*buffer).at(i) = HSV{0, 0, 0};
      continue;
    }

    if (direction == Direction::DESCENDING && i < currentCenter) {
      (*buffer).at(i) = HSV{0, 0, 0};
      continue;
    }

    int distance = std::abs(i - currentCenter);
    float relValue = value - pow(float(distance) / float(size), width);
    auto& p = (*buffer).at(i);
    (*buffer).at(i) = HSV{p.hue, p.saturation, std::clamp(relValue, 0.f, 1.f)};
  }

  return buffer;
}

void KnightRiderOperation::update(const size_t width, const size_t fps) {
  if (!isEnabled()) {
    return;
  }

  auto stepWitdh = width / fps / float(duration_milliseconds / 1000);

  if (direction == Direction::ASCENDING) {
    if (currentCenter >= width - 1) {
      direction = Direction::DESCENDING;
      return;
    } else {
      currentCenter += stepWitdh;
    }
  }

  if (direction == Direction::DESCENDING) {
    if (currentCenter <= 0) {
      direction = Direction::ASCENDING;
      return;
    } else {
      currentCenter -= stepWitdh;
    }
  }

  currentCenter = std::clamp(currentCenter, 0, int(width - 1));
}
