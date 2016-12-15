//
// Created by andi on 12/11/16.
//

#include "UDPInputOperation.h"


UDPInputOperation::UDPInputOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
    Operation("udpsink", store, start, end),
    sink(getValueByKey<int>("port", start, end))
{
    sink.start();
}

UDPInputOperation::~UDPInputOperation() {
    sink.stop();
}


void UDPInputOperation::operator()(const AbstractBaseBuffer <HSV> &buffer) {
    auto buf = sink.get();
    if (buf == nullptr)
        return;

    for (size_t i = 0; i < buffer.size() && i < buf->size(); i++) {
        const auto& e = buf->at(i);
        buffer.at(i) = e;
    }
}