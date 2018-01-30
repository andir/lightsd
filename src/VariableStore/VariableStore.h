#pragma once

#include <map>
#include <memory>
#include <set>
#include <shared_mutex>
#include <string>
#include "ValueType.h"

struct mqtt_var_type {
  std::string dataType;
  std::string format;
  std::string unit;
};

using MqttVarType = struct mqtt_var_type;

class VariableStore {
  mutable std::shared_mutex lock;
  std::map<std::string, std::weak_ptr<ValueType>> vars;
  std::map<std::string, MqttVarType> types;

 public:
  VariableStore();

  void registerVar(const std::string& name,
                   const MqttVarType& mqtt_type,
                   std::shared_ptr<ValueType> var);

  void unregisterVar(const std::string& name);

  void cleanUp();

  std::set<std::string> keys() const;

  std::shared_ptr<ValueType> getVar(const std::string& name) const;

  MqttVarType getMqttType(const std::string& name) const;
};
