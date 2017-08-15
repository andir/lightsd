#pragma once

#include <map>
#include <vector>
#include <memory>
#include "VariableStore/VariableStore.h"
#include "outputs/Output.h"
#include "operations/Operation.h"
#include "net/MqttConnection.h"

struct MqttConfig {
    const std::string broker;
    const std::string realm;
};

struct ConfigParsingException : public std::exception {
    const std::string s;

    explicit ConfigParsingException(const std::string& s) : s(s) {}

    const char *what() const throw() {
        return s.c_str();
    }
};

class Config {
public:
    Config();

    ~Config();

    size_t fps;
    size_t width;

    std::unique_ptr<MqttConnection> mqtt{nullptr};

    std::map<std::string, std::shared_ptr<Operation> > operations;
    std::vector<std::shared_ptr<Operation> > sequence;
    std::map<std::string, std::shared_ptr<Output> > outputs;
    std::shared_ptr<VariableStore> store;
};

typedef std::unique_ptr<Config> ConfigPtr;


ConfigPtr parseConfig(const std::string &filename);
