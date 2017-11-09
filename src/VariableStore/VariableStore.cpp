
#include "VariableStore.h"
#include <cassert>
#include <set>
#include <vector>

VariableStore::VariableStore() : lock() {}

void VariableStore::registerVar(const std::string& name,
                                const MqttVarType& mqtt_type,
                                std::shared_ptr<ValueType> var) {
  std::unique_lock<std::shared_mutex> locker(lock);
  auto weak_ptr = std::weak_ptr<ValueType>(var);

  if (vars.find(name) == vars.end()) {
    vars.emplace(name, weak_ptr);
  }

  if (types.find(name) == types.end()) {
    types.emplace(name, mqtt_type);
  }
}

void VariableStore::unregisterVar(const std::string& name) {
  std::unique_lock<std::shared_mutex> locker(lock);
  {
    auto it = vars.find(name);

    if (it != vars.end()) {
      vars.erase(it);
    }
  }
  {
    auto it = types.find(name);
    if (it != types.end()) {
      types.erase(it);
    }
  }
}

void VariableStore::cleanUp() {
  std::unique_lock<std::shared_mutex> locker(lock);
  std::vector<std::string> delete_list;

  for (const auto& it : vars) {
    const auto& key = it.first;
    const auto& value = it.second;

    auto spt = value.lock();

    if (spt == nullptr) {
      delete_list.push_back(key);
    }
  }

  for (const auto& e : delete_list) {
    vars.erase(e);
  }
}

std::set<std::string> VariableStore::keys() const {
  std::shared_lock<std::shared_mutex> locker(lock);
  std::set<std::string> list;

  for (const auto& e : vars) {
    list.insert(e.first);
  }

  return list;
}

std::shared_ptr<ValueType> VariableStore::getVar(
    const std::string& name) const {
  std::shared_lock<std::shared_mutex> locker(lock);
  auto it = vars.find(name);
  if (it != vars.end()) {
    auto spt = (*it).second.lock();
    // return spt, caller has to check for nullptr

    return spt;
  }
  return nullptr;
}

MqttVarType VariableStore::getMqttType(const std::string& name) const {
  std::shared_lock<std::shared_mutex> locker(lock);
  auto it = types.find(name);
  if (it != types.end()) {
    return (*it).second;
  }
  assert(false && "Invalid variable name");
}
