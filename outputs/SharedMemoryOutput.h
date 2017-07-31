#pragma once

#include <yaml-cpp/yaml.h>
#include "../SharedMemoryBuffer.h"
#include "Output.h"

class SharedMemoryOutput : public Output {
    const std::string filename;
    SharedMemoryBuffer shmBuffer;

public:
    explicit SharedMemoryOutput(const YAML::Node &params);

    virtual ~SharedMemoryOutput() {};

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer);

    virtual void draw(const std::vector<HSV> &buffer);
};
