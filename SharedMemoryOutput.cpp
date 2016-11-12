//
// Created by andi on 11/12/16.
//

#include "SharedMemoryOutput.h"

std::string parse_filename(const YAML::Node &params) {
    const auto f = params["filename"];
    if (f) return f.as<std::string>();
    return "/tmp/shm-leds";
}

int parse_size(const YAML::Node &params) {
    const auto f = params["size"];
    if (f) return f.as<int>();
    return 100;
}

SharedMemoryOutput::SharedMemoryOutput(const YAML::Node &params) : filename(parse_filename(params)),
                                                                   shmBuffer(parse_filename(params), parse_size(params)) {
}


void SharedMemoryOutput::draw(const AbstractBaseBuffer<HSV> &buffer) {
    auto it = buffer.begin();
    shmBuffer.resize(buffer.size());
    auto& buf = *shmBuffer.get();

    for (auto& e : buf) {
        if (it == buffer.end()) return;
        e = (*it).toRGB();
    }
}


void SharedMemoryOutput::draw(const std::vector<HSV>& buffer) {
    auto it = buffer.begin();
    shmBuffer.resize(buffer.size());
    auto& buf = *shmBuffer.get();

    for (auto& e : buf) {
        if (it == buffer.end()) return;
        e = (*it).toRGB();
    }
}