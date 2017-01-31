#ifndef LIGHTSD_UDPOUTPUTWRAPPER_H
#define LIGHTSD_UDPOUTPUTWRAPPER_H

#include <yaml-cpp/yaml.h>
#include "UDPOutput.h"
#include "Output.h"
#include "hsv.h"


class UDPOutputWrapper : public Output {
    UDPOutput output;
public:
    UDPOutputWrapper(const YAML::Node &params);

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer);

    virtual void draw(const std::vector <HSV> &buffer);
};

#endif //LIGHTSD_UDPOUTPUTWRAPPER_H
