#pragma once

#include <memory>

#include "Operation.h"

class SequenceOperation : public Operation {

        BoundConcreteValue<int> from;
        BoundConcreteValue<int> to;

        std::vector<std::unique_ptr<Operation> > sequence;


        void loadSequence(const std::string& name, VariableStore& s, YAML::const_iterator start, YAML::const_iterator end);

public:
        SequenceOperation(const std::string& name, VariableStore& s, YAML::const_iterator& start, YAML::const_iterator& end);

        void update(const size_t width, const size_t fps);
        Operation::BufferType operator()(Operation::BufferType& buffer);
};
