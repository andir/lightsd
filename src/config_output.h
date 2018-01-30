#pragma once

#include "VariableStore/VariableStore.h"
#include "outputs/Output.h"
#include <map>
#include <memory>
#include <yaml-cpp/yaml.h>

void parseOutputs(std::map<std::string, std::shared_ptr<Output>>& outputs,
                  const YAML::Node& outputs_node,
                  std::shared_ptr<VariableStore>& store);
