#pragma once

#include "operations/Operation.h"
#include <boost/algorithm/string.hpp>
#include <memory>
#include <yaml-cpp/yaml.h>

std::unique_ptr<Operation> generateOperation(
    const std::string& name,
    std::shared_ptr<VariableStore>& store,
    const YAML::Node& config_node);
