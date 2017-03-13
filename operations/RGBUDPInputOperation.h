//
// Created by andi on 12/11/16.
//

#ifndef LIGHTSD_UDPINPUTOPERATION_H
#define LIGHTSD_UDPINPUTOPERATION_H

#include "Operation.h"


#include <yaml-cpp/yaml.h>

#include "VariableStore/VariableStore.h"
#include "VariableStore/BoundConcreteValueType.h"


#include "HSVUDPSink.h"

class HSVUDPInputOperation : public Operation {

    HSVUDPSink sink;
public:
    HSVUDPInputOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end);
    ~HSVUDPInputOperation();

    void operator()(const AbstractBaseBuffer<HSV> &buffer);

};


#endif //LIGHTSD_UDPINPUTOPERATION_H
