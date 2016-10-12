//
// Created by andi on 10/12/16.
//
#include "WebsocketOutputWrapper.h"

#include <yaml-cpp/node/node.h>

int parse_port(const YAML::Node &params) {
    if (params["port"]) return params["port"].as<int>();
    return 9000;
}

WebsocketOutputWrapper::WebsocketOutputWrapper(const YAML::Node &params) : port(parse_port(params)),
                                                                           output(parse_port(params)) {

}


WebsocketOutputWrapper::~WebsocketOutputWrapper() {
    std::cerr << "Destruction WebsocketOutputWrapper" << std::endl;
}

void WebsocketOutputWrapper::draw(const AbstractBaseBuffer<HSV> &buffer) {
//    if (output == nullptr) {
//        output = std::make_unique<WebsocketOutput>(port);
//    }
    output.draw(buffer);
}

void WebsocketOutputWrapper::draw(const std::vector<HSV> &buffer) {
    output.draw(buffer);
}