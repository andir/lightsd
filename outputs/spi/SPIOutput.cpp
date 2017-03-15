//
// Created by andi on 3/15/17.
//

#include "SPIOutput.h"




SPIOutput::SPIOutput(const YAML::Node &params) : spi(params["device"].as<std::string>()) {}

void SPIOutput::draw(const AbstractBaseBuffer <HSV> &buffer) {
    _draw(buffer);
}

void SPIOutput::draw(const std::vector <HSV> &buffer) {
    _draw(buffer);
}