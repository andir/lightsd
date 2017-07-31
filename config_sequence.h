#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>
#include "operations/Operation.h"

std::unique_ptr<Operation>
generateSequenceStep(VariableStore &store, const std::string &step_type, YAML::const_iterator begin, YAML::const_iterator end);

