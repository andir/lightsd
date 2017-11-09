#include "RGBUDPInputOperation.h"

RGBUDPInputOperation::RGBUDPInputOperation(const std::string& name,
                                           std::shared_ptr<VariableStore> store,
                                           YAML::const_iterator start,
                                           YAML::const_iterator end)
    : Operation(name, store, start, end),
      sink(getValueByKey<int>("port", start, end)) {
  sink.start();
}

RGBUDPInputOperation::~RGBUDPInputOperation() {
  sink.stop();
}

Operation::BufferType RGBUDPInputOperation::operator()(
    Operation::BufferType& buffer) {
  auto buf = sink.get();
  if (buf == nullptr)
    return buffer;

  for (size_t i = 0; i < buffer->size() && i < buf->size(); i++) {
    const auto& e = buf->at(i);
    buffer->at(i) = e;
  }
  return buffer;
}
