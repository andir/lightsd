#pragma once

#include "../../rgb.h"
#include "../Output.h"
#include "SPI.h"
#include <yaml-cpp/yaml.h>

class SPIOutput : public Output {
  SPI spi;
  std::vector<RGB> output_buffer;

 public:
  explicit SPIOutput(const YAML::Node& params);

  void draw(const AbstractBaseBuffer<HSV>& buffer);

  void draw(const std::vector<HSV>& buffer);

 private:
  template <typename ContainerType>
  void _draw(const ContainerType& buffer) {
    if (output_buffer.capacity() < buffer.size()) {
      output_buffer.reserve(buffer.size());
    }

    size_t i = 0;
    for (const auto& e : buffer) {
      output_buffer[i++] = e.toRGB();
    }

    const uint8_t* const ptr = (const uint8_t*)output_buffer.data();
    spi.write(ptr, buffer.size());
  }
};
