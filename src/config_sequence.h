#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>
#include "operations/Operation.h"

std::unique_ptr<Operation>
generateOperation(const std::string& name, std::shared_ptr<VariableStore>& store, const YAML::Node &config_node);
