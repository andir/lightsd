//
// Created by andi on 11/12/16.
//

#ifndef LIGHTSD_SHAREDMEMORYOUTPUT_H
#define LIGHTSD_SHAREDMEMORYOUTPUT_H

#include <yaml-cpp/yaml.h>
#include "../DevMemBuffer.h"
#include "Output.h"

class DevMemOutput : public Output {
    const std::string filename;
    DevMemBuffer devMemBuffer;

public:
    DevMemOutput(const YAML::Node &params);

    virtual ~DevMemOutput() {};

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer);

    virtual void draw(const std::vector <HSV> &buffer);
};

#endif //LIGHTSD_SHAREDMEMORYOUTPUT_H
