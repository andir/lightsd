#include "HSVUDPInputOperation.h"


HSVUDPInputOperation::HSVUDPInputOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
    Operation("udpsink", store, start, end),
    sink(getValueByKey<int>("port", start, end))
{
    sink.start();
}

HSVUDPInputOperation::~HSVUDPInputOperation() {
    sink.stop();
}


void HSVUDPInputOperation::operator()(const AbstractBaseBuffer <HSV> &buffer) {
    if (!enabled.getInteger()) return;
    auto buf = sink.get();
    if (buf == nullptr)
        return;

    for (size_t i = 0; i < buffer.size() && i < buf->size(); i++) {
        const auto& e = buf->at(i);
        buffer.at(i) = e;
    }
}
