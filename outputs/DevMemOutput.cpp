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

    std::vector<size_t> parse_sizes(const YAML::Node &params) {
        const auto f = params["sizes"];
        if (f) return f.as<std::vector<size_t>>();
        return {100};
    }
}

DevMemOutput::DevMemOutput(const YAML::Node &params) : filename(parse_filename(params)),
                                                       devMemBuffer(parse_filename(params),
                                                                    parse_sizes(params)) {
}


void DevMemOutput::draw(const AbstractBaseBuffer<HSV> &buffer) {
    const auto &buf = *devMemBuffer.get();
    std::transform(buffer.begin(), buffer.end(), buf.begin(), [](const auto &p) {
        return p.toRGB();
    });
    devMemBuffer.writeSizes();
}


void DevMemOutput::draw(const std::vector<HSV> &buffer) {
    const auto &buf = *devMemBuffer.get();
    std::transform(buffer.begin(), buffer.end(), buf.begin(), [](const auto &p) {
        return p.toRGB();
    });
    devMemBuffer.writeSizes();
}
