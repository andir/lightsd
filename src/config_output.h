#pragma once

#include <yaml-cpp/yaml.h>
#include <map>
#include <memory>
#include "VariableStore/VariableStore.h"
#include "outputs/Output.h"

void parseOutputs(std::map<std::string, std::shared_ptr<Output>>& outputs,
                  const YAML::Node& outputs_node,
                  std::shared_ptr<VariableStore>& store);
