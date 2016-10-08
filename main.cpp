//
// Created by andi on 10/7/16.
//

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>

#include <boost/any.hpp>
#include <stdlib.h>
#include "buffer.h"
#include "hsv.h"
#include "algorithm.h"
#include "FrameScheduler.h"
#include "Output.h"
#include "WebsocketOutput.h"
#include "UDPOutput.h"


struct ConfigParsingException : public std::exception {
    const std::string s;

    ConfigParsingException(std::string s) : s(s) {}

    const char *what() const throw() {
        return s.c_str();
    }
};

class Operation {
public:
    Operation() {}

    virtual ~Operation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) = 0;
};

class RainbowOperation : public Operation {
public:
    RainbowOperation(YAML::const_iterator begin, YAML::const_iterator end) {
        // no parameters supported (yet)
    }

    virtual ~RainbowOperation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        algorithm::initRainbow(buffer);
    }
};


class RotateOperation : public Operation {
    size_t step;
    size_t step_width;
public:
    RotateOperation(YAML::const_iterator start, YAML::const_iterator end) : step(0) {
        for (; start != end; start++) {
            const auto iterator_value = *start;
            const std::string key = iterator_value.first.as<std::string>();

            if (key == "step_width") {
                step_width = iterator_value.second.as<size_t>();
            }
        }
    }

    virtual ~RotateOperation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {

        const size_t current_position = (step % buffer.count()) * step_width;
        const size_t offset = (current_position + step_width) % buffer.count();

        std::rotate(&buffer.at(0), &buffer.at(0) + offset, &*buffer.end());

        step++;
    }
};


class ShadeOperation : public Operation {
    float value;
public:
    ShadeOperation(YAML::const_iterator start, YAML::const_iterator end) : value(parse_value(start, end)) {}
    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        algorithm::MaskBuffer(value, buffer);
    }
private:
    float parse_value(YAML::const_iterator start, YAML::const_iterator end) {
        for (; start != end; start++) {
            const auto& v = *start;
            if (v.first.as<std::string>() == "value") {
                return v.second.as<float>();
            }
        }
        return 1.0f;
    }
};

class SolidColorOperation : public Operation {
    HSV color;
public:
    SolidColorOperation(YAML::const_iterator start, YAML::const_iterator end) : color(parse_color(start, end)) {}


    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        algorithm::initSolidColor(buffer, color);
    }

private:
    static HSV parse_color(YAML::const_iterator start, YAML::const_iterator end) {
        float hue = 0.0f, saturation = 0.0f, value = 0.0f;

        for (; start != end; start++) {
            const auto& v = *start;
            if (v.first.as<std::string>() == "hue") {
                hue = v.second.as<float>();
            } else if (v.first.as<std::string>() == "saturation") {
                saturation = v.second.as<float>();
            } else {
                value = v.second.as<float>();
            }
        }
        return HSV{hue, saturation, value};
    }
};



class FadeOperation : public Operation {
    const float min;
    const float max;
    const size_t from;
    const size_t to;

    std::vector<float> mask;

public:
    FadeOperation(YAML::const_iterator start, YAML::const_iterator end) :
            min(parse_min(start, end)),
            max(parse_max(start, end)),
            from(parse_from(start, end)),
            to(parse_to(start, end))
            {
                if (to <= from || min >= max)
                    return;

                const size_t length = (to - from);
                const float diff = max - min;
                const float step = diff / length;
                for(size_t i = 0; i < length; i++) {
                    mask.push_back(step * i);
                }
    }

    void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        algorithm::PartialMaskBuffer(mask, buffer, from);
    }
private:
    static float parse_min(YAML::const_iterator start, YAML::const_iterator end) {
        for(;start != end; start++) {
            const auto& e = *start;
            if (e.first.as<std::string>() == "min") {
                return e.second.as<float>();
            }
        }
        return 0.0f;
    }
    static float parse_max(YAML::const_iterator start, YAML::const_iterator end) {
        for(;start != end; start++) {
            const auto& e = *start;
            if (e.first.as<std::string>() == "max") {
                return e.second.as<float>();
            }
        }
        return 1.0f;
    }

    static size_t parse_to(YAML::const_iterator start, YAML::const_iterator end) {
        for(;start != end; start++) {
            const auto& e = *start;
            if (e.first.as<std::string>() == "to") {
                return e.second.as<size_t>();
            }
        }
        return 0;
    }

    static size_t parse_from(YAML::const_iterator start, YAML::const_iterator end) {
        for(;start != end; start++) {
            const auto& e = *start;
            if (e.first.as<std::string>() == "from") {
                return e.second.as<size_t>();
            }
        }
        return 0;
    }
};

template<typename IteratorType1, typename IteratorType2>
std::unique_ptr<Operation> generateSequenceStep(const std::string &step_type, IteratorType1 begin, IteratorType2 end) {
    using namespace std::string_literals;

    static const std::set<std::string> known_sequence_types = {
            "initrainbow"s, "rotate"s, "initsolidcolor"s, "shade"s, "fade"s
    };


    const auto lower_case_name = boost::algorithm::to_lower_copy(step_type);

    const auto it = known_sequence_types.find(lower_case_name);

    if (it == known_sequence_types.end()) {
        throw ConfigParsingException("The selected step_type wasn't found.");
    } else {
        if (lower_case_name == "initrainbow") {
            return std::make_unique<RainbowOperation>(begin, end);
        } else if (lower_case_name == "rotate") {
            return std::make_unique<RotateOperation>(begin, end);
        } else if (lower_case_name == "initsolidcolor") {
            return std::make_unique<SolidColorOperation>(begin, end);
        } else if (lower_case_name == "shade") {
            return std::make_unique<ShadeOperation>(begin, end);
        } else if (lower_case_name == "fade") {
            return std::make_unique<FadeOperation>(begin, end);
        } else {
            return nullptr;
        }
    }

};


class WebsocketOutputWrapper : public Output {
    std::unique_ptr<WebsocketOutput> output;
    int port;
public:
    WebsocketOutputWrapper(const YAML::Node& params) : port(parse_port(params)), output(nullptr) {

    }

    ~WebsocketOutputWrapper() {
        std::cerr << "Destruction WebsocketOutputWrapper" << std::endl;
    }

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer) {
        if (output == nullptr) {
            output = std::make_unique<WebsocketOutput>(port);
        }
        output->draw(buffer);
    }

    virtual void draw(const std::vector<HSV> &buffer) {
        if (output == nullptr) {
            output = std::make_unique<WebsocketOutput>(port);
        }
        output->draw(buffer);
    }

    static int parse_port(const YAML::Node& params) {
        if (params["port"]) return params["port"].as<int>();
        return 9000;
    }
};


class UDPOutputWrapper : public Output {
    UDPOutput output;
public:
    UDPOutputWrapper(const YAML::Node& params) : output(parse_host(params), parse_port(params)) {}


    virtual void draw(const AbstractBaseBuffer<HSV> &buffer) {
        output.draw(buffer);
    }

    virtual void draw(const std::vector<HSV> &buffer) {
        output.draw(buffer);
    }
private:


    static std::string parse_host(const YAML::Node& params) {
        return params["host"].as<std::string>();
    }

    static std::string parse_port(const YAML::Node& params) {
        if (params["port"]) {
            return params["port"].as<std::string>();
        }
        return "6453";
    }
};


struct Config {
    size_t fps;
    size_t width;

    std::vector<std::unique_ptr<Operation> > sequence;
    std::map<std::string, std::shared_ptr<Output> > outputs;
};


void parseSequence(std::vector<std::unique_ptr<Operation>> &steps, const YAML::Node &sequence_node) {
    assert(sequence_node.Type() == YAML::NodeType::Sequence);

    for (const auto node : sequence_node) {
        switch (node.Type()) {
            case YAML::NodeType::Scalar: {
                const std::string step_name = node.as<std::string>();

                auto step = generateSequenceStep(step_name, node.end(),
                                                 node.end());
                steps.push_back(std::move(step));
                break;

            }
            case YAML::NodeType::Map: {

                for (const auto &map_entry  : node) {
                    const std::string step_name = map_entry.first.as<std::string>();
                    std::cout << step_name << std::endl;


                    switch (map_entry.second.Type()) {
                        case YAML::NodeType::Map: {
                            auto step = generateSequenceStep(step_name, map_entry.second.begin(),
                                                             map_entry.second.end());
                            steps.push_back(std::move(step));
                            break;
                        }
                        default:
                            std::cout << map_entry.second.Type() << std::endl;
                            throw ConfigParsingException("Invalid node in sequence.");
                    }

                }

                break;
            }
            case YAML::NodeType::Sequence:
            case YAML::NodeType::Undefined:
            case YAML::NodeType::Null:
                throw ConfigParsingException("Invalid node in sequence.");
        }
    }
}


void parseOutputs(std::map<std::string, std::shared_ptr<Output>> &outputs, const YAML::Node &outputs_node) {
    using namespace std::string_literals;
    assert(outputs_node.Type() == YAML::NodeType::Map);

    for (const auto node : outputs_node) {
        const std::string output_name = node.first.as<std::string>();

        if (outputs.find(output_name) != outputs.end()) {
            throw ConfigParsingException("Output already known.");
        }

        if (node.second.Type() != YAML::NodeType::Map) {
            throw ConfigParsingException("Outputs must be formatted as dict.");
        }


        static const std::set<std::string> known_types = {
                "websocket"s,
                "udp"s,
        };
        const std::string type = node.second["type"].as<std::string>();

        if (known_types.find(type) == known_types.end()) {
            throw ConfigParsingException("Unknown output type");
        }

        const auto params = node.second["params"];

        if (params.Type() != YAML::NodeType::Map) {
            throw ConfigParsingException("Params must be formatted as dict.");
        }

        if (type == "websocket") {
            auto p = std::make_unique<WebsocketOutputWrapper>(params);
            outputs[output_name] = std::move(p);
        } else if (type == "udp") {
            auto p = std::make_unique<UDPOutputWrapper>(params);
            outputs[output_name] = std::move(p);
        } else {
            throw ConfigParsingException("Unknown output type.");
        }

    }
}

typedef std::unique_ptr<Config> ConfigPtr;

class WorkerThread  {
    bool doRun;
    ConfigPtr config_ptr;
    ConfigPtr new_config_ptr;
    std::mutex config_mutex;

public:
    WorkerThread() : config_ptr(nullptr), doRun(true) {

    }

    void setConfig(ConfigPtr cfg) {
        std::lock_guard<std::mutex> lock(config_mutex);
        new_config_ptr = std::move(cfg);
    }

    ConfigPtr getConfig() {
        std::lock_guard<std::mutex> lock(config_mutex);
        if (new_config_ptr != nullptr) {
            return std::move(new_config_ptr);
        }
        return nullptr;
    }


    void run() {
        while (doRun) {
            ConfigPtr config = getConfig();
            while (config == nullptr) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                config = std::move(getConfig());
            };

            FrameScheduler scheduler(config->fps);
            AllocatedBuffer<HSV> buffer(config->width);

            while (true) {
                if (new_config_ptr.get() != nullptr) {
                    break;
                }

                Frame frame(scheduler);

                for (const auto &step : config->sequence) {
                    (*step)(buffer);
                }
                for (const auto &output : config->outputs) {
                    (*output.second).draw(buffer);
                }

            }
        }
    }


};


ConfigPtr parseConfig(const std::string& filename) {
    auto config = std::make_unique<Config>();

    YAML::Node yaml_config = YAML::LoadFile(filename);

    if (yaml_config["width"]) {
        config->width = yaml_config["width"].as<size_t>();
    } else {
        config->width = 15;
    }

    if (yaml_config["fps"]) {
        config->fps = yaml_config["fps"].as<size_t>();
    } else {
        config->fps = 25;
    }

    if (!yaml_config["sequence"]) {
        throw ConfigParsingException("No sequence configured.");
    } else {
        const YAML::Node sequence_node = yaml_config["sequence"];
        parseSequence(config->sequence, sequence_node);
    }

    if (!yaml_config["outputs"]) {
        throw ConfigParsingException("No outputs defined.");
    } else {
        const YAML::Node outputs_node = yaml_config["outputs"];
        parseOutputs(config->outputs, outputs_node);

    }
    return config;

}



static WorkerThread workerThread;
static std::string config_filename;


void emptySignalHandler(int signum) {

}

void sigHupHandler(int signum) {
    if (signum == SIGHUP) {
        static std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        auto conf = parseConfig(config_filename);
        workerThread.setConfig(std::move(conf));
    } else {
        exit(signum);
    }
}

int main(const int argc, const char *argv[]) {

    config_filename = "config.yml";

    if (argc > 1) {
        config_filename = std::string(argv[1]);
    }

    workerThread.setConfig(parseConfig(config_filename));

    sigset_t set;
    sigemptyset(&set);
    sigprocmask(SIG_SETMASK, &set, NULL);
    if (pthread_sigmask(SIG_SETMASK, &set, NULL)) {
        std::cerr << "failed to set SIGHUP mask" << std::endl;
        return 1;
    }

    std::thread t([](){
        sigset_t set;
        sigemptyset(&set);
        sigprocmask(SIG_SETMASK, &set, NULL);
        if (pthread_sigmask(SIG_SETMASK, &set, NULL)) {
            std::cerr << "failed to set SIGHUP mask" << std::endl;
            return 1;
        }
        workerThread.run();
    });

    if (pthread_sigmask(SIG_BLOCK, &set, NULL)) {
        std::cerr << "failed to set SIGHUP mask" << std::endl;
        return 1;
    }

    signal(SIGHUP, sigHupHandler);

    t.join();

    return 0;
}