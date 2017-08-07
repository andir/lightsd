#pragma once

#include <yaml-cpp/yaml.h>
#include "Output.h"

class WebsocketOutputWrapper : public Output {
    // internal pointer to the websocket output
    std::unique_ptr<Output> output;
    int port;
public:
    explicit WebsocketOutputWrapper(const YAML::Node &params);

    ~WebsocketOutputWrapper();

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer);

    virtual void draw(const std::vector<HSV> &buffer);

};
