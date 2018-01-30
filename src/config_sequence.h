#pragma once

#include <yaml-cpp/yaml.h>
#include <boost/algorithm/string.hpp>
#include <memory>
#include "operations/Operation.h"

std::unique_ptr<Operation> generateOperation(
    const std::string& name,
    std::shared_ptr<VariableStore>& store,
    const YAML::Node& config_node);
