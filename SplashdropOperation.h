//
// Created by andi on 12/4/16.
//

#ifndef LIGHTSD_SPLASHDROPOPERATION_H
#define LIGHTSD_SPLASHDROPOPERATION_H

#include <yaml-cpp/yaml.h>


#include "operations.h"
#include "VariableStore/VariableStore.h"
#include "VariableStore/BoundConcreteValueType.h"

class SplashdropOperation : public Operation {

    enum STATE {
        IDLE = 0,
        DROPPING,
        RISING_SPLASH,
        DEACYING_SPLASH
    };

    struct Drop {
        HSV color;
        float rate;
        STATE state;
    };


    std::vector<Drop> drops;


    void hitDrop(Drop& drop);
    void decayDrop(Drop& drop);
    void drawSplash(const Drop &drop,const size_t index, const AbstractBaseBuffer<HSV> &buffer, const size_t width);
    void drawDrop(Drop&, const size_t index, const AbstractBaseBuffer<HSV> &buffer);

public:
    SplashdropOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end);

    void draw(const AbstractBaseBuffer<HSV> &buffer);

    void operator()(const AbstractBaseBuffer<HSV> &buffer);

};


#endif //LIGHTSD_SPLASHDROPOPERATION_H
