#include "SequenceOperation.h"
#include "../config_sequence.h"
#include "../config.h"

SequenceOperation::SequenceOperation(const std::string& name, VariableStore& s, YAML::const_iterator& start, YAML::const_iterator& end) : 
        Operation(name, s, start, end),
        from(name + "/from", Operation::INT, s, getValueByKey<int>("from", start, end, 0)),
        to(name + "/to", Operation::INT, s, getValueByKey<int>("to", start, end, 5))
{

    for(auto it = start; it != end; it++) {
       const auto& key = (*it).first.as<std::string>();
       if (key == "operations") {
           if (!(*it).second.IsSequence()) {
              throw ConfigParsingException("operations must be a sequence."); 
           }
           loadSequence(name, s, (*it).second.begin(), (*it).second.end());
           break;
       }
    }
}


void SequenceOperation::loadSequence(const std::string& name, VariableStore& s, YAML::const_iterator start, YAML::const_iterator end) {
        for (auto it = start; it != end; it++) {
                auto val = *it;
                if (!val.IsMap()) {
                        throw ConfigParsingException("Each operation should be defined with a map");
                }
                const auto op_name = val["name"].as<std::string>();
                auto op = generateOperation(name + ":" + op_name, s, val);
                sequence.push_back(std::move(op));
                std::cerr << op_name << " added" << std::endl;
        }
}

Operation::BufferType SequenceOperation::operator()(Operation::BufferType& buffer) {
        Operation::BufferType b = std::make_shared<BufferView<typename Operation::ContainerType>>(buffer, from.getInteger(), to.getValue());   
        for (auto& op : sequence) {
                b = (*op)(b);
        }
        return buffer;
}