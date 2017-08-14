#pragma once

#include <string>
#include <memory>
#include <shared_mutex>
#include <map>
#include <set>
#include "ValueType.h"

class VariableStore {
    mutable std::shared_mutex lock;
    std::map<std::string, std::weak_ptr<ValueType>> vars;
    std::map<std::string, std::string> types;

public:

    VariableStore();

    void registerVar(const std::string& name, const std::string& type_name, std::shared_ptr<ValueType> var);

    void unregisterVar(const std::string& name);

    void cleanUp();

    std::set<std::string> keys() const;

    std::shared_ptr<ValueType> getVar(const std::string& name) const;

    std::string getTypeName(const std::string& name) const;

};
