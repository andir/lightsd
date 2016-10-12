//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_WEBSOCKETOUTPUTWRAPPER_H
#define LIGHTSD_WEBSOCKETOUTPUTWRAPPER_H


#include <yaml-cpp/yaml.h>
#include "Output.h"
#include "WebsocketOutput.h"

class WebsocketOutputWrapper : public Output {
    WebsocketOutput output;
    int port;
public:
    WebsocketOutputWrapper(const YAML::Node &params);

    ~WebsocketOutputWrapper();

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer);
    virtual void draw(const std::vector<HSV> &buffer);

};


#endif //LIGHTSD_WEBSOCKETOUTPUTWRAPPER_H
