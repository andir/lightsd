#include "HSVUDPInputOperation.h"

HSVUDPInputOperation::HSVUDPInputOperation(const std::string& name,
                                           std::shared_ptr<VariableStore> store,
                                           YAML::const_iterator start,
                                           YAML::const_iterator end)
    : Operation(name, store, start, end),
      sink(getValueByKey<int>("port", start, end)),
      started(false) {}

HSVUDPInputOperation::~HSVUDPInputOperation() {
  sink.stop();
}

void HSVUDPInputOperation::update(const size_t, const size_t) {
  if (started)
    return;
  sink.start();
  started = true;
}

Operation::BufferType HSVUDPInputOperation::operator()(
    Operation::BufferType& buffer) {
  auto buf = sink.get();
  if (buf == nullptr)
    return buffer;

  for (size_t i = 0; i < (*buffer).size() && i < buf->size(); i++) {
    const auto& e = buf->at(i);
    (*buffer).at(i) = e;
  }
  return buffer;
}
