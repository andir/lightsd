#pragma once

#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "operations/Operation.h"

void
parseSequence(VariableStore &store, std::vector<std::unique_ptr<Operation>> &steps, const YAML::Node &sequence_node);

