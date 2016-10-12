//
// Created by andi on 10/12/16.
//

#include "UDPOutputWrapper.h"

namespace {
    static std::string parse_host(const YAML::Node &params) {
        return params["host"].as<std::string>();
    }

    static std::string parse_port(const YAML::Node &params) {
        if (params["port"]) {
            return params["port"].as<std::string>();
        }
        return "6453";
    }
}


UDPOutputWrapper::UDPOutputWrapper(const YAML::Node &params) : output(parse_host(params), parse_port(params)) {}

void UDPOutputWrapper::draw(const AbstractBaseBuffer <HSV> &buffer) {
    output.draw(buffer);
}

void UDPOutputWrapper::draw(const std::vector<HSV> &buffer) {
    output.draw(buffer);
}

