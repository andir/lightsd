#pragma once

#include <yaml-cpp/yaml.h>
#include "../DevMemBuffer.h"
#include "Output.h"

class DevMemOutput : public Output {
    const std::string filename;
    DevMemBuffer devMemBuffer;

public:
    explicit DevMemOutput(const YAML::Node &params);

    virtual ~DevMemOutput() {};

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer);

    virtual void draw(const std::vector<HSV> &buffer);
};
