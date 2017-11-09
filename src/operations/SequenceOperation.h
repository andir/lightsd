#pragma once

#include <memory>

#include "../utils/JobQueue.h"
#include "Operation.h"

class SequenceOperation : public Operation {
  JobQueue job_queue;
  BoundConcreteValue<int> from;
  BoundConcreteValue<int> to;

  std::vector<std::unique_ptr<Operation>> sequence;
  void loadSequence(const std::string& name,
                    std::shared_ptr<VariableStore>& s,
                    YAML::const_iterator start,
                    YAML::const_iterator end);

 public:
  SequenceOperation(const std::string& name,
                    std::shared_ptr<VariableStore>& s,
                    YAML::const_iterator& start,
                    YAML::const_iterator& end);

  void update(const size_t width, const size_t fps);
  Operation::BufferType operator()(Operation::BufferType& buffer);
};
