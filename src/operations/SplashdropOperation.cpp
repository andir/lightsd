//
// Created by andi on 12/4/16.
//

#include "SplashdropOperation.h"
#include <cassert>
#include "../utils/random.h"

SplashdropOperation::SplashdropOperation(const std::string& name,
                                         std::shared_ptr<VariableStore> store,
                                         YAML::const_iterator start,
                                         YAML::const_iterator end)
    : Operation(name, store, start, end) {}

void SplashdropOperation::draw(const AbstractBaseBuffer<HSV>& buffer) {}

void SplashdropOperation::hitDrop(Drop& drop) {
  if (drop.state != IDLE)
    return;

  // TODO: add random colors
  drop.color = HSV{
      200,
      1.0f,
      0.0f,
  };
  drop.rate = 0.3f;
  drop.state = DROPPING;
}

void SplashdropOperation::decayDrop(Drop& drop) {
  if (drop.color.value == 0.0f) {
    return;
  }

  drop.color.value *= 1.0f - drop.rate;
}

void SplashdropOperation::drawSplash(const Drop& drop,
                                     const size_t index,
                                     const AbstractBaseBuffer<HSV>& buffer,
                                     const size_t width) {
  size_t left = width;
  size_t right = width;

  if (width >= index) {
    if (index > 0) {
      left = index - 1;
    } else {
      left = 0;
    }
  }

  if (index + width >= buffer.size()) {
    right = buffer.size() - index;
  }

  for (size_t i = 0; i < width; i++) {
    auto e = drop.color;
    e.value -= i * drop.rate;
    if (i < left) {
      auto& b = buffer.at(index - i);
      b = e;
    }

    if (i < right) {
      auto& b = buffer.at(index + i);
      b = e;
    }
  }
}

void SplashdropOperation::drawDrop(Drop& drop,
                                   const size_t index,
                                   const AbstractBaseBuffer<HSV>& buffer) {
  if (drop.state != IDLE) {
    return;
  }

  buffer.at(index) = drop.color;

  switch (drop.state) {
    case DROPPING:
      drop.color.value += drop.rate;
      if (drop.color.value >= 1.0f) {
        drop.state = RISING_SPLASH;
        drop.color.value = 1.0f;
      }
      break;
    case RISING_SPLASH: {
      drop.color.value *= 1.0f - drop.rate;
      const size_t width = drop.rate / drop.color.value;

      drawSplash(drop, index, buffer, width);

      if (drop.color.value <= 0.5f) {
        drop.color.value = 0.5f;
        drop.state = DEACYING_SPLASH;
      }
      break;
    }
    case DEACYING_SPLASH: {
      drop.color.value *= 1.0f - drop.rate;
      const size_t width = drop.color.value / drop.rate;

      drawSplash(drop, index, buffer, width);

      if (drop.color.value <= 0.0f) {
        drop.state = IDLE;
      }

      break;
    }
    case IDLE:
    default:
      break;
  }

  buffer.at(index) = drop.color;
}

Operation::BufferType SplashdropOperation::operator()(
    Operation::BufferType& buffer) {
  if (drops.size() != (*buffer).size()) {
    std::cerr << "resizing()" << std::endl;
    drops.clear();
    drops.resize((*buffer).size());
    for (auto& drop : drops) {
      drop.color.value = 0;
      drop.state = IDLE;
    }
  }

  size_t i = 0;
  for (auto& drop : drops) {
    const float chance = 0.001;
    static const auto max_roll = 10000;
    const auto roll = random_int_in_range(0, max_roll);
    const auto bound = chance * max_roll;

    if (roll < bound) {
      hitDrop(drop);
      std::cout << "hit()" << i << std::endl;
    }

    drawDrop(drop, i++, *buffer);
  }

  return buffer;
}
