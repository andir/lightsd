//
// Created by andi on 10/11/16.
//

#ifndef VARIABLESTORE_VARIABLESTORE_H
#define VARIABLESTORE_VARIABLESTORE_H

#include <string>
#include <memory>
#include <shared_mutex>
#include <map>
#include "ValueType.h"

class VariableStore {
    mutable std::shared_mutex lock;
    std::map<std::string, std::weak_ptr<ValueType>> vars;

public:

    VariableStore();

    void registerVar(const std::string name, std::shared_ptr<ValueType> var);
    void unregisterVar(const std::string name);
    void cleanUp();

    std::set<std::string> keys() const;

    std::shared_ptr<ValueType> getVar(const std::string name) const;

};


#endif //VARIABLESTORE_VARIABLESTORE_H
