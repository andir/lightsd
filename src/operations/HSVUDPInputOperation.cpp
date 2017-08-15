#include "HSVUDPInputOperation.h"


HSVUDPInputOperation::HSVUDPInputOperation(const std::string& name, VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
        Operation(name, store, start, end),
        sink(getValueByKey<int>("port", start, end)) {
    sink.start();
}

HSVUDPInputOperation::~HSVUDPInputOperation() {
    sink.stop();
}


Operation::BufferType HSVUDPInputOperation::operator()(Operation::BufferType &buffer) {
    auto buf = sink.get();
    if (buf == nullptr)
        return buffer;

    for (size_t i = 0; i < (*buffer).size() && i < buf->size(); i++) {
        const auto &e = buf->at(i);
        (*buffer).at(i) = e;
    }
    return buffer;
}
