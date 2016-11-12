//
// Created by andi on 11/12/16.
//

#include "SharedMemoryOutput.h"
#include <algorithm>

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
    shmBuffer.ensureSize(buffer.size());
    const auto& buf = *shmBuffer.get();
    const auto old_counter = buf.at(0).empty;

    std::transform(buffer.begin(), buffer.end(), buf.begin(), [](const auto & p){
        return p.toRGB();
    });
    auto& val = buf.at(0).empty;
    val = (old_counter + 1) % 255;
}


void SharedMemoryOutput::draw(const std::vector<HSV>& buffer) {
    shmBuffer.ensureSize(buffer.size());
    const auto& buf = *shmBuffer.get();
    const auto old_counter = buf.at(0).empty;

    std::transform(buffer.begin(), buffer.end(), buf.begin(), [](const auto & p){
        return p.toRGB();
    });
    auto& val = buf.at(0).empty;
    val = (old_counter + 1) % 255;
}