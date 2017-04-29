//
// Created by andi on 11/12/16.
//

#include "DevMemOutput.h"
#include <algorithm>
#include <yaml-cpp/yaml.h>

namespace {
std::string parse_filename(const YAML::Node &params) {
    const auto f = params["filename"];
    if (f) return f.as<std::string>();
    return "/dev/mem";
}

int parse_size(const YAML::Node &params) {
    const auto f = params["size"];
    if (f) return f.as<int>();
    return 100;
}
}

DevMemOutput::DevMemOutput(const YAML::Node &params) : filename(parse_filename(params)),
                                                                   devMemBuffer(parse_filename(params),
                                                                             parse_size(params)) {
}


void DevMemOutput::draw(const AbstractBaseBuffer<HSV> &buffer) {
    devMemBuffer.ensureSize(buffer.size());
    const auto &buf = *devMemBuffer.get();
    std::transform(buffer.begin(), buffer.end(), buf.begin(), [](const auto &p) {
        return p.toRGB();
    });
}


void DevMemOutput::draw(const std::vector <HSV> &buffer) {
    devMemBuffer.ensureSize(buffer.size());
    const auto &buf = *devMemBuffer.get();
    std::transform(buffer.begin(), buffer.end(), buf.begin(), [](const auto &p) {
        return p.toRGB();
    });
}
