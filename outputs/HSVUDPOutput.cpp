//
// Created by andi on 10/3/16.
//

#include "HSVUDPOutput.h"


inline std::string
getValueOrDefault(const std::string& name, const YAML::Node &params, const std::string& default_value) {
    if (params[name]) {
        return params[name].as<std::string>();
    }
    return default_value;
}


HSVUDPOutput::HSVUDPOutput(const YAML::Node &params)
        : HSVUDPOutput(getValueOrDefault("host", params, "127.0.0.1"), getValueOrDefault("port", params, "1338")) {
}

HSVUDPOutput::HSVUDPOutput(const std::string& destination, const std::string& port) :
        socket(io_service, udp::endpoint(udp::v4(), 0)),
        resolver(io_service) {
    endpoint = *resolver.resolve({udp::v4(), destination, port});
}

void HSVUDPOutput::draw(const std::vector<HSV> &buffer) {
    _send<std::vector<HSV> >(buffer);
}

void HSVUDPOutput::draw(const AbstractBaseBuffer<HSV> &buffer) {
    _send<AbstractBaseBuffer<HSV>>(buffer);
}
