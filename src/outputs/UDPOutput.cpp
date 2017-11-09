//
// Created by andi on 10/3/16.
//

#include "UDPOutput.h"

UDPOutput::UDPOutput(const std::string& destination, const std::string& port)
    : socket(io_service, udp::endpoint(udp::v4(), 0)), resolver(io_service) {
  endpoint = *resolver.resolve({udp::v4(), destination, port});
}

void UDPOutput::draw(const std::vector<HSV>& buffer) {
  _send<std::vector<HSV>>(buffer);
}

void UDPOutput::draw(const AbstractBaseBuffer<HSV>& buffer) {
  _send<AbstractBaseBuffer<HSV>>(buffer);
}
