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
#include "operations.h"
#include "VariableStore/VariableStore.h"
#include "MqttVarStore.h"


struct ConfigParsingException : public std::exception {
    const std::string s;

    ConfigParsingException(std::string s) : s(s) {}

    const char *what() const throw() {
        return s.c_str();
    }
};


template<typename IteratorType1, typename IteratorType2>
std::unique_ptr<Operation> generateSequenceStep(VariableStore& store, const std::string &step_type, IteratorType1 begin, IteratorType2 end) {
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
            return std::make_unique<RainbowOperation>(store, begin, end);
        } else if (lower_case_name == "rotate") {
            return std::make_unique<RotateOperation>(store, begin, end);
        } else if (lower_case_name == "initsolidcolor") {
            return std::make_unique<SolidColorOperation>(store, begin, end);
        } else if (lower_case_name == "shade") {
            return std::make_unique<ShadeOperation>(store, begin, end);
        } else if (lower_case_name == "fade") {
            return std::make_unique<FadeOperation>(store, begin, end);
        } else {
            return nullptr;
        }
    }
};


class WebsocketOutputWrapper : public Output {
    std::unique_ptr<WebsocketOutput> output;
    int port;
public:
    WebsocketOutputWrapper(const YAML::Node &params) : port(parse_port(params)), output(nullptr) {

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

    static int parse_port(const YAML::Node &params) {
        if (params["port"]) return params["port"].as<int>();
        return 9000;
    }
};


class UDPOutputWrapper : public Output {
    UDPOutput output;
public:
    UDPOutputWrapper(const YAML::Node &params) : output(parse_host(params), parse_port(params)) {}

    virtual void draw(const AbstractBaseBuffer<HSV> &buffer) {
        output.draw(buffer);
    }

    virtual void draw(const std::vector<HSV> &buffer) {
        output.draw(buffer);
    }

private:

    static std::string parse_host(const YAML::Node &params) {
        return params["host"].as<std::string>();
    }

    static std::string parse_port(const YAML::Node &params) {
        if (params["port"]) {
            return params["port"].as<std::string>();
        }
        return "6453";
    }
};


class Config {
public:

    ~Config() {
        sequence.clear();
        outputs.clear();
    }

    size_t fps;
    size_t width;

    std::vector<std::unique_ptr<Operation> > sequence;
    std::map<std::string, std::shared_ptr<Output> > outputs;
    VariableStore store;
};


void parseSequence(VariableStore& store, std::vector<std::unique_ptr<Operation>> &steps, const YAML::Node &sequence_node) {
    assert(sequence_node.Type() == YAML::NodeType::Sequence);

    for (const auto node : sequence_node) {
        switch (node.Type()) {
            case YAML::NodeType::Scalar: {
                const std::string step_name = node.as<std::string>();

                auto step = generateSequenceStep(store, step_name, node.end(),
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
                            auto step = generateSequenceStep(store, step_name, map_entry.second.begin(),
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

class WorkerThread {
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
            MqttVarStore mqtt(config->store, "whisky.trollhoehle.net", "/test/");



            for (const auto& key : config->store.keys() ) {
                std::cout << key << " " << *config->store.getVar(key) << std::endl;
            }

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


ConfigPtr parseConfig(const std::string &filename) {
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
        parseSequence(config->store, config->sequence, sequence_node);
    }

    if (!yaml_config["outputs"]) {
        throw ConfigParsingException("No outputs defined.");
    } else {
        const YAML::Node outputs_node = yaml_config["outputs"];
        parseOutputs(config->outputs, outputs_node);

    }

    return config;
}


static WorkerThread worker_thread;
static std::string config_filename;


void sigHupHandler(int signum) {
    if (signum == SIGHUP) {
        static std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        auto conf = parseConfig(config_filename);
        worker_thread.setConfig(std::move(conf));
    } else {
        exit(signum);
    }
}

int main(const int argc, const char *argv[]) {
    config_filename = "config.yml";

    if (argc > 1) {
        config_filename = std::string(argv[1]);
    }

    worker_thread.setConfig(parseConfig(config_filename));


    std::thread t([]() {
//        sigset_t set;
//        sigemptyset(&set);
//        sigprocmask(SIG_SETMASK, &set, NULL);
//        if (pthread_sigmask(SIG_SETMASK, &set, NULL)) {
//            std::cerr << "failed to set SIGHUP mask" << std::endl;
//            return 1;
//        }
        worker_thread.run();
        return 0;
    });

    signal(SIGHUP, sigHupHandler);

    t.join();

    return 0;
}