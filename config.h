//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_CONFIG_H
#define LIGHTSD_CONFIG_H

#include <map>
#include <vector>
#include <memory>
#include "VariableStore/VariableStore.h"
#include "operations.h"
#include "Output.h"

class Config {
public:

    inline ~Config() {
        sequence.clear();
        outputs.clear();
    }

    size_t fps;
    size_t width;

    std::vector<std::unique_ptr<Operation> > sequence;
    std::map<std::string, std::shared_ptr<Output> > outputs;
    VariableStore store;
};

typedef std::unique_ptr<Config> ConfigPtr;


ConfigPtr parseConfig(const std::string &filename);


#endif //LIGHTSD_CONFIG_H
