#pragma once

#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "operations/Operation.h"
#include <boost/algorithm/string.hpp>

std::unique_ptr<Operation>
generateOperation(const std::string& name, VariableStore &store, const YAML::Node &config_node);
