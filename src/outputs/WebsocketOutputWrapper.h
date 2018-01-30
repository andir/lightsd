#pragma once

#include <VariableStore/VariableStore.h>
#include <yaml-cpp/yaml.h>
#include <memory>
#include "Output.h"

class WebsocketOutputWrapper : public Output {
  // internal pointer to the websocket output
  std::unique_ptr<Output> output;
  int port;

 public:
  explicit WebsocketOutputWrapper(const YAML::Node& params,
                                  std::shared_ptr<VariableStore>& store);

  ~WebsocketOutputWrapper();

  virtual void draw(const AbstractBaseBuffer<HSV>& buffer);

  virtual void draw(const std::vector<HSV>& buffer);
};
