#include "SequenceOperation.h"
#include "../config.h"
#include "../config_sequence.h"

SequenceOperation::SequenceOperation(const std::string& name,
                                     std::shared_ptr<VariableStore>& s,
                                     YAML::const_iterator& start,
                                     YAML::const_iterator& end)
    : Operation(name, s, start, end),
      from(name + "/from",
           Operation::INT,
           s,
           getValueByKey<int>("from", start, end, 0)),
      to(name + "/to",
         Operation::INT,
         s,
         getValueByKey<int>("to", start, end, 5)) {
  for (auto it = start; it != end; it++) {
    const auto& key = (*it).first.as<std::string>();
    if (key == "operations") {
      if (!(*it).second.IsSequence()) {
        throw ConfigParsingException("operations must be a sequence.");
      }
      loadSequence(name, s, (*it).second.begin(), (*it).second.end());
      break;
    }
  }
}

void SequenceOperation::loadSequence(const std::string& name,
                                     std::shared_ptr<VariableStore>& s,
                                     YAML::const_iterator start,
                                     YAML::const_iterator end) {
  for (auto it = start; it != end; it++) {
    auto val = *it;
    if (!val.IsMap()) {
      throw ConfigParsingException(
          "Each operation should be defined with a map");
    }
    const auto op_name = val["name"].as<std::string>();
    auto op = generateOperation(name + ":" + op_name, s, val);
    sequence.push_back(std::move(op));
    std::cerr << op_name << " added" << std::endl;
  }
}

void SequenceOperation::update(const size_t width, const size_t fps) {
  const size_t size = to - from;
  job_queue.execute(sequence, size, fps);
}

Operation::BufferType SequenceOperation::operator()(
    Operation::BufferType& buffer) {
  Operation::BufferType b =
      std::make_shared<BufferView<typename Operation::ContainerType>>(
          buffer, from.getInteger(), to.getValue() - from.getValue());
  for (auto& op : sequence) {
    if (op->isEnabled())
      b = (*op)(b);
  }
  return buffer;
}
