//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_CONFIG_H
#define LIGHTSD_CONFIG_H

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

    ConfigParsingException(std::string s) : s(s) {}

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

    std::vector<std::unique_ptr<Operation> > sequence;
    std::map<std::string, std::shared_ptr<Output> > outputs;
    std::shared_ptr<VariableStore> store;
};

typedef std::unique_ptr<Config> ConfigPtr;


ConfigPtr parseConfig(const std::string &filename);


#endif //LIGHTSD_CONFIG_H
