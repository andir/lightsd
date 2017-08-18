#pragma once

#include "Operation.h"

#include <yaml-cpp/yaml.h>

#include "../VariableStore/VariableStore.h"
#include "../VariableStore/BoundConcreteValueType.h"

#include "../net/HSVUDPSink.h"

class HSVUDPInputOperation : public Operation {

    HSVUDPSink sink;
    bool started;
public:
    HSVUDPInputOperation(const std::string& name, VariableStore &store, YAML::const_iterator start, YAML::const_iterator end);

    ~HSVUDPInputOperation();

    void update(const size_t, const size_t);
    Operation::BufferType operator()(Operation::BufferType &buffer);

};
