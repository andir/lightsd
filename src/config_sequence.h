#pragma once

#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "operations/Operation.h"

std::unique_ptr<Operation>
generateOperation(VariableStore &store, const YAML::Node &config_node);

void
parseSequence(VariableStore &store, std::vector<std::unique_ptr<Operation>> &steps, const YAML::Node &sequence_node);

